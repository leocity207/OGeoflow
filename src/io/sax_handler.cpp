#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <format>

#include "include/io/sax_handler.h"
#include "include/io/exception.h"

#include "include/geojson/json_value.h"
#include "include/geojson/position.h"
#include "include/geojson/feature.h"
#include "include/geojson/geojson.h"
#include "include/geojson/geometry.h"
#include "include/geojson/geometry/geometry_collection.h"
#include "include/geojson/geometry/line_string.h"
#include "include/geojson/geometry/multi_line_string.h"
#include "include/geojson/geometry/multi_point.h"
#include "include/geojson/geometry/multi_polygone.h"
#include "include/geojson/geometry/point.h"
#include "include/geojson/geometry/polygone.h"


IO::GeoJSON_SAX_Handler::GeoJSON_SAX_Handler() : 
	m_parsing_successful(false),
	m_in_coordinates(false),
	m_coordinate_depth(0)
{
	Push_Context(Parse_State::ROOT);
}

GeoJSON::GeoJSON&& IO::GeoJSON_SAX_Handler::Get_Geojson() 
{ 
	return std::move(m_geojson);
}

bool IO::GeoJSON_SAX_Handler::Is_Valid() const 
{ 
	return m_parsing_successful;
}

GeoJSON::Geometry_Type IO::GeoJSON_SAX_Handler::String_To_Geometry_Type(const std::string& type_str)
{
	if (type_str == "Point") return GeoJSON::Geometry_Type::POINT;
	if (type_str == "MultiPoint") return GeoJSON::Geometry_Type::MULTI_POINT;
	if (type_str == "LineString") return GeoJSON::Geometry_Type::LINE_STRING;
	if (type_str == "MultiLineString") return GeoJSON::Geometry_Type::MULTI_LINE_STRING;
	if (type_str == "Polygon") return GeoJSON::Geometry_Type::POLYGON;
	if (type_str == "MultiPolygon") return GeoJSON::Geometry_Type::MULTI_POLYGON;
	if (type_str == "GeometryCollection") return GeoJSON::Geometry_Type::GEOMETRY_COLLECTION;
	return GeoJSON::Geometry_Type::UNKNOWN;
}

GeoJSON::GeoJSON::Type String_To_GeoJSON_Type(const std::string& type_str)
{
	if (type_str == "FeatureCollection") return GeoJSON::GeoJSON::Type::FEATURE_COLLECTION;
	if (type_str == "Feature") return GeoJSON::GeoJSON::Type::FEATURE;
	if (type_str == "Geometry") return GeoJSON::GeoJSON::Type::GEOMETRY;
	throw IO::Exception("Uknown type of Geojson type");
}


void IO::GeoJSON_SAX_Handler::Validate_Geojson_Type(const std::string& type)
{
	IO::Ensure( type == "FeatureCollection" || 
				type == "Feature"         || 
				type == "Point"   || 
				type == "MultiPoint"   ||
				type == "LineString"        || 
				type == "MultiLineString" || 
				type == "Polygon" || 
				type == "MultiPolygon" || 
				type == "GeometryCollection",
				std::format("Invalid GeoJSON type: {}",type));
}

// Validate geometry type
void IO::GeoJSON_SAX_Handler::Validate_Geometry_Type(const std::string& type) {
	IO::Ensure(String_To_Geometry_Type(type) != GeoJSON::Geometry_Type::UNKNOWN, std::format("Invalid geometry type: {}",type));
}



void IO::GeoJSON_SAX_Handler::Push_Context(Parse_State state)
{
	m_context_stack.push_back({state, "", GeoJSON::JSON_Value()});
}

void IO::GeoJSON_SAX_Handler::Pop_Context()
{
	if (!m_context_stack.empty())
		m_context_stack.pop_back();
}

IO::GeoJSON_SAX_Handler::Parse_State IO::GeoJSON_SAX_Handler::Current_Context() const
{
	return m_context_stack.empty() ? Parse_State::UNKNOWN : m_context_stack.back().state;
}

void IO::GeoJSON_SAX_Handler::Set_Current_Key(const std::string& key)
{
	if (!m_context_stack.empty())
		m_context_stack.back().current_key = key;
}

bool IO::GeoJSON_SAX_Handler::StartObject()
{
	auto current = Current_Context();
	
	switch (current) {
		case IO::GeoJSON_SAX_Handler::Parse_State::ROOT: return true;
		case IO::GeoJSON_SAX_Handler::Parse_State::FEATURE_COLLECTION:
			if (m_context_stack.back().current_key == "features")
			{
				// Starting a new Feature
				m_geojson.features.emplace_back();
				Push_Context(Parse_State::FEATURE);
			}
			break;
		case IO::GeoJSON_SAX_Handler::Parse_State::FEATURE:
			if (m_context_stack.back().current_key == "geometry")
				Push_Context(Parse_State::GEOMETRY);
			else if (m_context_stack.back().current_key == "properties")
				Push_Context(Parse_State::PROPERTIES);
			break;
			
		case IO::GeoJSON_SAX_Handler::Parse_State::PROPERTIES:
			// Nested object in properties
			m_context_stack.back().current_value = GeoJSON::JSON_Object();
			break;
			
		default:
			break;
	}
	
	return true;
}

std::unique_ptr<GeoJSON::Geometry> IO::GeoJSON_SAX_Handler::Parse_Geometry_Coordinates(const std::string& geometry_type, const GeoJSON::JSON_Array& coordinates_array)
{
    
    if (geometry_type == "Point")
        return Parse_Point_Coordinates(coordinates_array);
    else if (geometry_type == "MultiPoint")
        return Parse_MultiPoint_Coordinates(coordinates_array);
    else if (geometry_type == "LineString")
        return Parse_LineString_Coordinates(coordinates_array);
    else if (geometry_type == "MultiLineString")
        return Parse_MultiLineString_Coordinates(coordinates_array);
    else if (geometry_type == "Polygon")
        return Parse_Polygon_Coordinates(coordinates_array);
    else if (geometry_type == "MultiPolygon")
        return Parse_MultiPolygon_Coordinates(coordinates_array);
    else
        throw IO::Exception(std::format("Unsupported geometry type: {}", geometry_type));
}

std::unique_ptr<GeoJSON::Point> IO::GeoJSON_SAX_Handler::Parse_Point_Coordinates(const GeoJSON::JSON_Array& coordinates)
{
    IO::Ensure(coordinates.size() >= 2 && coordinates.size() <= 3, 
               "Point coordinates must have 2 or 3 elements");
    IO::Ensure(coordinates[0].is_number() && coordinates[1].is_number(), 
               "Point coordinates must be numbers");
    
    double lon = coordinates[0].as_number();
    double lat = coordinates[1].as_number();
    
    // Validate coordinate ranges
    IO::Ensure(lon >= -180.0 && lon <= 180.0, "Longitude must be between -180 and 180");
    IO::Ensure(lat >= -90.0 && lat <= 90.0, "Latitude must be between -90 and 90");
    
    if (coordinates.size() == 3) {
        IO::Ensure(coordinates[2].is_number(), "Altitude must be a number");
        return std::make_unique<GeoJSON::Point>(GeoJSON::Position(lon, lat, coordinates[2].as_number()));
	}
    else
        return std::make_unique<GeoJSON::Point>(GeoJSON::Position(lon, lat));
}

std::unique_ptr<GeoJSON::Multi_Point> IO::GeoJSON_SAX_Handler::Parse_MultiPoint_Coordinates(const GeoJSON::JSON_Array& coordinates)
{
    auto multi_point = std::make_unique<GeoJSON::Multi_Point>();
    
    for (const auto& point_coords : coordinates) {
        IO::Ensure(point_coords.is_array(), "MultiPoint elements must be coordinate arrays");
        auto point = Parse_Point_Coordinates(point_coords.as_array());
        multi_point->positions.push_back(point->position);
    }
    
    return multi_point;
}

std::unique_ptr<GeoJSON::Line_String> IO::GeoJSON_SAX_Handler::Parse_LineString_Coordinates(const GeoJSON::JSON_Array& coordinates)
{
    auto line_string = std::make_unique<GeoJSON::Line_String>();
    
    for (const auto& position_coords : coordinates) {
        IO::Ensure(position_coords.is_array(), "LineString elements must be coordinate arrays");
        auto point = Parse_Point_Coordinates(position_coords.as_array());
        line_string->positions.push_back(point->position);
    }
    
    IO::Ensure(line_string->positions.size() >= 2, "LineString must have at least 2 positions");
    return line_string;
}

std::unique_ptr<GeoJSON::Multi_Line_String> IO::GeoJSON_SAX_Handler::Parse_MultiLineString_Coordinates(const GeoJSON::JSON_Array& coordinates)
{
    auto multi_line_string = std::make_unique<GeoJSON::Multi_Line_String>();
    
    for (const auto& line_coords : coordinates) {
        IO::Ensure(line_coords.is_array(), "MultiLineString elements must be arrays of coordinates");
        auto line_string = Parse_LineString_Coordinates(line_coords.as_array());
        multi_line_string->lines.push_back(std::move(*line_string));
    }
    
    return multi_line_string;
}

std::unique_ptr<GeoJSON::Polygon> IO::GeoJSON_SAX_Handler::Parse_Polygon_Coordinates(const GeoJSON::JSON_Array& coordinates)
{
    auto polygon = std::make_unique<GeoJSON::Polygon>();
    
    for (const auto& ring_coords : coordinates) {
        IO::Ensure(ring_coords.is_array(), "Polygon elements must be arrays of coordinates");
        auto line_string = Parse_LineString_Coordinates(ring_coords.as_array());
        
        // Validate ring is closed (first and last position equal)
        IO::Ensure(line_string->positions.size() >= 4, "Polygon ring must have at least 4 positions");
        const auto& first = line_string->positions.front();
        const auto& last = line_string->positions.back();
        IO::Ensure(std::abs(first.longitude - last.longitude) <= 1e-10 && std::abs(first.latitude - last.latitude) <= 1e-10, "Polygon ring must be closed (first and last position equal)");
        
        polygon->rings.push_back(std::move(line_string->positions));
    }
    
    IO::Ensure(!polygon->rings.empty(), "Polygon must have at least one ring");
    return polygon;
}

std::unique_ptr<GeoJSON::Multi_Polygon> IO::GeoJSON_SAX_Handler::Parse_MultiPolygon_Coordinates(const GeoJSON::JSON_Array& coordinates)
{
    auto multi_polygon = std::make_unique<GeoJSON::Multi_Polygon>();
    
    for (const auto& polygon_coords : coordinates) {
        IO::Ensure(polygon_coords.is_array(), "MultiPolygon elements must be arrays of rings");
        auto polygon = Parse_Polygon_Coordinates(polygon_coords.as_array());
        multi_polygon->polygons.push_back(std::move(*polygon));
    }
    
    return multi_polygon;
}

bool IO::GeoJSON_SAX_Handler::EndObject(rapidjson::SizeType /*elementCount*/)
{
	auto current = Current_Context();
	auto& context = m_context_stack.back();
	
	switch (current)
	{
		case Parse_State::ROOT:
			m_parsing_successful = true;
			break;
			
		case Parse_State::FEATURE:
			// Validate feature has required fields
			Pop_Context();
			break;
			
		case Parse_State::GEOMETRY: {
			auto& geometry_value = context.current_value;
			IO::Ensure(geometry_value.is_object(), "Geometry must be an object");
			
			const auto& geom_obj = geometry_value.as_object();
			auto type_it = geom_obj.find("type");
			IO::Ensure(type_it != geom_obj.end(), "Geometry must have 'type' property");
			IO::Ensure(type_it->second.is_string(), "Geometry type must be a string");
			
			std::string geom_type = type_it->second.as_string();
			Validate_Geometry_Type(geom_type);
			
			if (geom_type == "GeometryCollection") {
				auto geometries_it = geom_obj.find("geometries");
				IO::Ensure(geometries_it != geom_obj.end(), "GeometryCollection must have 'geometries' property");
				IO::Ensure(geometries_it->second.is_array(), "GeometryCollection geometries must be an array");
				
				if (!m_geojson.features.empty())
					m_geojson.features.back().geometry = Create_Geometry_Collection(geometries_it->second.as_array());
				else
					m_geojson.geometry = Create_Geometry_Collection(geometries_it->second.as_array());
			}
			else
			{
				auto coords_it = geom_obj.find("coordinates");
				IO::Ensure(coords_it != geom_obj.end(), std::format("Geometry type '{}' must have 'coordinates' property", geom_type));
				IO::Ensure(coords_it->second.is_array(), "Geometry coordinates must be an array");
				
				if (!m_geojson.features.empty())
					m_geojson.features.back().geometry = Parse_Geometry_Coordinates(geom_type, coords_it->second.as_array());
				else
					m_geojson.geometry = Parse_Geometry_Coordinates(geom_type, coords_it->second.as_array());
			}
			Pop_Context();
			break;
		}
			
		case Parse_State::PROPERTIES:
			if (!m_geojson.features.empty())
				m_geojson.features.back().properties = context.current_value.as_object();
			Pop_Context();
			break;
			
		default:
			Pop_Context();
			break;
	}
	
	return true;
}

bool IO::GeoJSON_SAX_Handler::StartArray() {
	auto current = Current_Context();
	
	switch (current)
	{
		case Parse_State::FEATURE_COLLECTION:
			if (m_context_stack.back().current_key == "features") {
				// Features array started
			}
			break;
			
		case Parse_State::GEOMETRY:
			if (m_context_stack.back().current_key == "coordinates")
			{
				m_in_coordinates = true;
				m_coordinate_depth = 1;
				m_coordinate_buffer.clear();
			   	m_number_buffer.clear();
				m_ring_buffer.clear();
				m_polygon_buffer.clear();
			}
			else if (m_context_stack.back().current_key == "geometries")
			{
				Push_Context(Parse_State::GEOMETRY_COLLECTION);
				m_context_stack.back().current_value = GeoJSON::JSON_Array();
			}
			break;
		case Parse_State::COORDINATES:
			m_coordinate_depth++;
			
			// Initialize buffers for nested arrays
			if (m_coordinate_depth == 2)
				m_ring_buffer.push_back(std::vector<GeoJSON::Position>());
			else if (m_coordinate_depth == 3)
			{
				if (!m_ring_buffer.empty())
					m_ring_buffer.back().clear();
				m_polygon_buffer.push_back(std::vector<std::vector<GeoJSON::Position>>());
			}
			break;
			
		case Parse_State::BBOX:
			m_number_buffer.clear();
			break;
			
		default:
			break;
	}
	
	// Handle array value creation
	if (current == Parse_State::PROPERTIES || 
		(current == Parse_State::GEOMETRY && m_context_stack.back().current_key == "coordinates")) {
		m_context_stack.back().current_value = GeoJSON::JSON_Array();
	}
	
	return true;
}

bool IO::GeoJSON_SAX_Handler::EndArray(rapidjson::SizeType /*elementCount*/)
{
	auto current = Current_Context();
	
	switch (current) {
		case Parse_State::GEOMETRY:
			if (m_context_stack.back().current_key == "coordinates") {
				m_in_coordinates = false;
				m_coordinate_depth = 0;
				Finalize_Coordinates();
			}
			break;
		case Parse_State::COORDINATES:
			m_coordinate_depth--;
			
			// Handle nested array completion
			if (m_coordinate_depth == 1 && !m_coordinate_buffer.empty()) {
				if (m_current_geometry_type == "MultiLineString" || 
					m_current_geometry_type == "Polygon") {
					m_ring_buffer.push_back(m_coordinate_buffer);
					m_coordinate_buffer.clear();
				}
			} else if (m_coordinate_depth == 2 && !m_ring_buffer.empty()) {
				if (m_current_geometry_type == "MultiPolygon") {
					m_polygon_buffer.push_back(m_ring_buffer);
					m_ring_buffer.clear();
				}
			}
			break;
			
		case Parse_State::GEOMETRY_COLLECTION: {
			// Handle GeometryCollection geometries array
			auto geometries_array = m_context_stack.back().current_value.as_array();
			Pop_Context();
			
			if (!m_geojson.features.empty())
				m_geojson.features.back().geometry = Create_Geometry_Collection(geometries_array);
			break;
		}
		case Parse_State::BBOX:
			IO::Ensure(m_number_buffer.size() == 4 || m_number_buffer.size() == 6, "BBox must have 4 or 6 elements");
			m_geojson.bbox = m_number_buffer;
			break;
		default:
			break;
	}
	
	return true;
}

bool IO::GeoJSON_SAX_Handler::Key(const char* str, rapidjson::SizeType length, bool /*copy*/)
{
	std::string key(str, length);
	Set_Current_Key(key);
	
	auto current = Current_Context();
	
	switch (current) {
		case Parse_State::ROOT:
			if (key == "type") {}
				// Will be handled in String method
			else if (key == "features")
				m_geojson.type = GeoJSON::GeoJSON::Type::FEATURE_COLLECTION;
			else if (key == "bbox")
				Push_Context(Parse_State::BBOX);
			break;
			
		case Parse_State::FEATURE:
			IO::Ensure(key == "type" || key == "id" || key == "geometry" || key == "properties", std::format("Invalid key in Feature: {} ",key));
			break;
			
		case Parse_State::GEOMETRY:
			IO::Ensure(key == "type" || key == "coordinates" || key == "geometries", std::format("Invalid key in Feature: {} ",key));
			if (key == "type" || key == "coordinates" || key == "geometries") { }
			break;
			
		default:
			break;
	}
	
	return true;
}

bool IO::GeoJSON_SAX_Handler::String(const char* str, rapidjson::SizeType length, bool /*copy*/) {
	std::string value(str, length);
	auto& context = m_context_stack.back();
	
	switch (Current_Context()) {
		case Parse_State::ROOT:
			if (context.current_key == "type") {
				Validate_Geojson_Type(value);
				m_geojson.type = String_To_GeoJSON_Type(value);
				
				if (value == "FeatureCollection")
					Push_Context(Parse_State::FEATURE_COLLECTION);
				else if (value == "Feature") {
					// Single Feature at root
					m_geojson.features.emplace_back();
					Push_Context(Parse_State::FEATURE);
				} else // Single Geometry at root
					Push_Context(Parse_State::GEOMETRY);
			}
			break;
			
		case Parse_State::FEATURE:
			if (context.current_key == "type")
				IO::Ensure(value == "Feature", "Feature type must be 'Feature'");
			else if (context.current_key == "id")
				m_geojson.features.back().id = value;
			break;
			
		case Parse_State::GEOMETRY:
			if (context.current_key == "type")
			{
				Validate_Geometry_Type(value);
				m_current_geometry_type = value;
				auto geom_obj = context.current_value.as_object();
				geom_obj["type"] = value;
			}
			break;
			
		case Parse_State::PROPERTIES:
			context.current_value = value;
			break;
			
		default:
			break;
	}
	
	return true;
}

bool IO::GeoJSON_SAX_Handler::Bool(bool value) {
	if (Current_Context() == Parse_State::PROPERTIES)
		m_context_stack.back().current_value = value;
	return true;
}

bool IO::GeoJSON_SAX_Handler::Double(double value) {
	auto current = Current_Context();
	
	if (current == Parse_State::PROPERTIES)
		m_context_stack.back().current_value = value;
	else if (current == Parse_State::BBOX)
		m_number_buffer.push_back(value);
	else if (m_in_coordinates)
		Process_Coordinate_Number(value);
	
	return true;
}

// Implement other numeric types to handle coordinates
bool IO::GeoJSON_SAX_Handler::Int(int value) 
{ 
	return Double(static_cast<double>(value));
}

bool IO::GeoJSON_SAX_Handler::Uint(unsigned value) 
{ 
	return Double(static_cast<double>(value));
}

bool IO::GeoJSON_SAX_Handler::Int64(int64_t value) 
{ 
	return Double(static_cast<double>(value));
}

bool IO::GeoJSON_SAX_Handler::Uint64(uint64_t value) 
{
	 return Double(static_cast<double>(value));
}

bool IO::GeoJSON_SAX_Handler::RawNumber(const char* str, rapidjson::SizeType length, bool copy) {
	// Convert string to double
	try {
		double value = std::stod(std::string(str, length));
		return Double(value);
	} catch (const std::exception&) {
		return false;
	}
}

bool IO::GeoJSON_SAX_Handler::Null() {
	if (Current_Context() == Parse_State::PROPERTIES)
		m_context_stack.back().current_value = GeoJSON::JSON_Value();
	else if (Current_Context() == Parse_State::FEATURE &&  m_context_stack.back().current_key == "geometry")
		if (!m_geojson.features.empty()) // Geometry can be null
			m_geojson.features.back().geometry.reset();
	return true;
}

// Parse a single position from array
void IO::GeoJSON_SAX_Handler::Parse_Position(const GeoJSON::JSON_Array& arr) {
	IO::Ensure(arr.size() >= 2, "Position must have at least 2 elements");
	IO::Ensure(arr[0].is_number() && arr[1].is_number(), "Position coordinates must be numbers");
	
	double lon = arr[0].as_number();
	double lat = arr[1].as_number();
	
	// Validate longitude and latitude ranges
	IO::Ensure(lon >= -180.0 && lon <= 180.0, "Longitude must be between -180 and 180");
	IO::Ensure(lat >= -90.0 && lat <= 90.0,"Latitude must be between -90 and 90");
	
	GeoJSON::Position pos;
	pos.longitude = lon;
	pos.latitude = lat;
	
	if (arr.size() >= 3) {
		IO::Ensure(arr[2].is_number(), "Altitude must be a number");
		pos.altitude = arr[2].as_number();
	}
	
	m_coordinate_buffer.push_back(pos);
}

void IO::GeoJSON_SAX_Handler::Process_Coordinate_Number(double value) {
	if (!m_in_coordinates) return;
	
	m_number_buffer.push_back(value);
	
	// When we have 2 or 3 numbers, we have a complete position
	if (m_number_buffer.size() >= 2) {
		GeoJSON::Position pos;
		pos.longitude = m_number_buffer[0];
		pos.latitude = m_number_buffer[1];
		
		if (m_number_buffer.size() >= 3) {
			pos.altitude = m_number_buffer[2];
		}
		
		m_coordinate_buffer.push_back(pos);
		m_number_buffer.clear();
	}
}

void IO::GeoJSON_SAX_Handler::Finalize_Coordinates() {
	if (!m_geojson.features.empty() && !m_coordinate_buffer.empty()) {
		auto& current_feature = m_geojson.features.back();
		
		if (m_current_geometry_type == "Point") 
			if (m_coordinate_buffer.size() == 1)
				current_feature.geometry = std::make_unique<GeoJSON::Point>(m_coordinate_buffer[0]);
		else if (m_current_geometry_type == "MultiPoint")
			current_feature.geometry = Create_Multi_Point();
		else if (m_current_geometry_type == "LineString")
			current_feature.geometry = Create_Line_String();
		else if (m_current_geometry_type == "MultiLineString")
			current_feature.geometry = Create_Multi_Line_String();
		else if (m_current_geometry_type == "Polygon")
			current_feature.geometry = Create_Polygon();
		else if (m_current_geometry_type == "MultiPolygon")
			current_feature.geometry = Create_Multi_Polygon();
	}
	
	m_coordinate_buffer.clear();
	m_ring_buffer.clear();
	m_polygon_buffer.clear();
	m_number_buffer.clear();
}

std::unique_ptr<GeoJSON::Point> IO::GeoJSON_SAX_Handler::Create_Point() {
	IO::Ensure(m_coordinate_buffer.size() == 1, "Point geometry requires exactly one position");
	return std::make_unique<GeoJSON::Point>(m_coordinate_buffer[0]);
}

std::unique_ptr<GeoJSON::Multi_Point> IO::GeoJSON_SAX_Handler::Create_Multi_Point() {
	auto multi_point = std::make_unique<GeoJSON::Multi_Point>();
	multi_point->positions = m_coordinate_buffer;
	return multi_point;
}

std::unique_ptr<GeoJSON::Line_String> IO::GeoJSON_SAX_Handler::Create_Line_String() {
	auto line_string = std::make_unique<GeoJSON::Line_String>();
	line_string->positions = m_coordinate_buffer;
	
	// Validate LineString has at least 2 positions
	IO::Ensure(line_string->positions.size() >= 2, "LineString must have at least 2 positions");
	
	return line_string;
}

std::unique_ptr<GeoJSON::Multi_Line_String> IO::GeoJSON_SAX_Handler::Create_Multi_Line_String() {
	auto multi_line_string = std::make_unique<GeoJSON::Multi_Line_String>();
	
	// For MultiLineString, we need to process ring_buffer which contains multiple lines
	for (const auto& line_positions : m_ring_buffer) {
		IO::Ensure(line_positions.size() >= 2, "Each LineString in MultiLineString must have at least 2 positions");
		
		GeoJSON::Line_String line_string;
		line_string.positions = line_positions;
		multi_line_string->lines.push_back(std::move(line_string));
	}
	
	return multi_line_string;
}

std::unique_ptr<GeoJSON::Polygon> IO::GeoJSON_SAX_Handler::Create_Polygon() {
	auto polygon = std::make_unique<GeoJSON::Polygon>();
	
	// For Polygon, ring_buffer contains the rings (first is exterior, rest are holes)
	for (const auto& ring_positions : m_ring_buffer) {
		IO::Ensure(ring_positions.size() >= 4, "Polygon ring must have at least 4 positions (closed ring)");

		
		// Check if ring is closed (first and last position should be equal)
		const auto& first = ring_positions.front();
		const auto& last = ring_positions.back();
		if (std::abs(first.longitude - last.longitude) > 1e-10 || std::abs(first.latitude - last.latitude) > 1e-10) throw IO::Exception("Polygon ring must be closed (first and last position equal)");
		
		polygon->rings.push_back(ring_positions);
	}
	
	IO::Ensure(!polygon->rings.empty(), "Polygon must have at least one ring");
	return polygon;
}

std::unique_ptr<GeoJSON::Multi_Polygon> IO::GeoJSON_SAX_Handler::Create_Multi_Polygon() {
	auto multi_polygon = std::make_unique<GeoJSON::Multi_Polygon>();
	
	// For MultiPolygon, polygon_buffer contains multiple polygons
	for (const auto& polygon_rings : m_polygon_buffer) {
		GeoJSON::Polygon polygon;
		
		for (const auto& ring_positions : polygon_rings) {
			IO::Ensure(ring_positions.size() >= 4, "Polygon ring must have at least 4 positions");
			
			// Check if ring is closed
			const auto& first = ring_positions.front();
			const auto& last = ring_positions.back();
			IO::Ensure(std::abs(first.longitude - last.longitude) <= 1e-10 && std::abs(first.latitude - last.latitude) <= 1e-10, "Polygon ring must be closed");
			
			polygon.rings.push_back(ring_positions);
		}
		
		IO::Ensure(!polygon.rings.empty(), "Each Polygon in MultiPolygon must have at least one ring");
		
		multi_polygon->polygons.push_back(std::move(polygon));
	}
	
	return multi_polygon;
}

std::unique_ptr<GeoJSON::Geometry_Collection> IO::GeoJSON_SAX_Handler::Create_Geometry_Collection(const GeoJSON::JSON_Array& geometries_array) {
    auto geometry_collection = std::make_unique<GeoJSON::Geometry_Collection>();
    
    for (const auto& geom_value : geometries_array) {
        IO::Ensure(geom_value.is_object(), "Geometry in GeometryCollection must be an object");
        
        const auto& geom_obj = geom_value.as_object();
        auto type_it = geom_obj.find("type");
        IO::Ensure(type_it != geom_obj.end(), "Geometry in GeometryCollection must have 'type' property");
        IO::Ensure(type_it->second.is_string(), "Geometry type must be a string");
        
        std::string geom_type = type_it->second.as_string();
        Validate_Geometry_Type(geom_type);
        
        // Recursively parse the geometry
        if (geom_type == "GeometryCollection") {
            auto geometries_it = geom_obj.find("geometries");
            IO::Ensure(geometries_it != geom_obj.end(), "GeometryCollection must have 'geometries' property");
            IO::Ensure(geometries_it->second.is_array(), "GeometryCollection geometries must be an array");
            geometry_collection->geometries.push_back(Create_Geometry_Collection(geometries_it->second.as_array()));
        }
		else
		{
            auto coords_it = geom_obj.find("coordinates");
            IO::Ensure(coords_it != geom_obj.end(), std::format("Geometry type '{}' must have 'coordinates' property", geom_type));
            IO::Ensure(coords_it->second.is_array(), "Geometry coordinates must be an array");
            
            geometry_collection->geometries.push_back(Parse_Geometry_Coordinates(geom_type, coords_it->second.as_array()));
        }
    }
    
    return geometry_collection;
}
