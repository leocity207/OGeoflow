#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <format>
#include <ranges>

#include "include/io/sax_handler.h"
#include "include/io/error.h"

#include "include/geojson/properties.h"
#include "include/geojson/position.h"
#include <optional>
#include <geojson/geometry_type/geometry_collection.h>

using Level1 = GeoJSON::Position;
using Level2 = std::vector<GeoJSON::Position>;
using Level3 = std::vector<std::vector<GeoJSON::Position>>;
using Level4 = std::vector<GeoJSON::Polygon>;


IO::GeoJSON_SAX_Handler::GeoJSON_SAX_Handler() :
	m_current_error(IO::Error::Type::NO_ERROR),
	m_level(0),
	m_max_level(0),
	m_add_level(0)
{
	Push_Context(Parse_State::ROOT);
}

bool IO::GeoJSON_SAX_Handler::Push_Context(Parse_State state,GeoJSON::Property& ref_property, std::string_view key)
{
	switch (state)
	{
		case Parse_State::FEATURE:
			m_id = std::nullopt;
			break;
		case Parse_State::COORDINATES:
			m_coordinate = GeoJSON::Position();
			m_positions.Clear();
			m_max_level = 0;
			m_level = 0;
			m_add_level = 0;
			break;
	}
	m_context_stack.emplace_back( ref_property, state, std::string(key),GeoJSON::Key::FOREIGN);
	return true;
}

bool IO::GeoJSON_SAX_Handler::Reset_State(Parse_State state)
{
	Current_Context().state = state;
	return true;
}

IO::GeoJSON_SAX_Handler::Parse_Context IO::GeoJSON_SAX_Handler::Pop_Context()
{
	assert(!m_context_stack.empty());
	IO::GeoJSON_SAX_Handler::Parse_Context context = std::move(m_context_stack.back());
	m_context_stack.pop_back();
	return context;
}

IO::GeoJSON_SAX_Handler::Parse_State IO::GeoJSON_SAX_Handler::Current_State() const
{
	assert(!m_context_stack.empty());
	return m_context_stack.back().state;
}

IO::GeoJSON_SAX_Handler::Parse_Context& IO::GeoJSON_SAX_Handler::Current_Context()
{
	assert(!m_context_stack.empty());
	return m_context_stack.back();
}

bool IO::GeoJSON_SAX_Handler::In_Array()
{
	switch (Current_State())
	{
		case Parse_State::PROPERTIES_SUB_ARRAY:
		case Parse_State::FOREIGN_ARRAY:
		case Parse_State::COORDINATES:
		case Parse_State::FEATURE_COLLECTION:
		case Parse_State::GEOMETRY_COLLECTION:
		case Parse_State::BBOX:
			return true;
		default:
			return false;
	}
}

GeoJSON::Key IO::GeoJSON_SAX_Handler::Set_Current_Key(std::string_view key)
{
	if (!m_context_stack.empty())
	{
		m_context_stack.back().key = GeoJSON::String_To_Key(key);
		return m_context_stack.back().key;
	}
	return GeoJSON::Key::FOREIGN;
}

bool IO::GeoJSON_SAX_Handler::StartObject()
{
	switch (Current_State())
	{
		case Parse_State::ROOT:
		case Parse_State::PROPERTIES_OBJECT:    return true;
		case Parse_State::PROPERTIES_SUB_ARRAY:
		{
			if (Current_Context().property.get().Is_Array())
			{
				Current_Context().property.get().Get_Array().emplace_back(GeoJSON::Property::Object());
				return Push_Context(Parse_State::PROPERTIES_OBJECT, Current_Context().property.get().Get_Array().back());
			}
			return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
		}
		case Parse_State::PROPERTIES_SUB_KEY:
		{
			if (Current_Context().property.get().Is_Object())
			{
				if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
					return Push_Error(IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);
				Current_Context().property.get().Get_Object()[Current_Context().key_str] = GeoJSON::Property::Object();
				Reset_State(Parse_State::PROPERTIES_OBJECT);
				Current_Context().property = Current_Context().property.get().Get_Object()[Current_Context().key_str];
				return true;
			}
			return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
		}
		case Parse_State::FOREIGN_KEY:          return Reset_State(Parse_State::FOREIGN_OBJECT);
		case Parse_State::FOREIGN_ARRAY:        return Push_Context(Parse_State::FOREIGN_OBJECT);
		case Parse_State::FEATURE_COLLECTION:   return Push_Context(Parse_State::FEATURE);
		case Parse_State::GEOMETRY_COLLECTION:  return Push_Context(Parse_State::GEOMETRY);
		case Parse_State::GEOMETRY: return true;
		case Parse_State::FEATURE:  return true;
	}
	return Push_Error(IO::Error::Type::UNEXPECTED_STATE_OBJECT);
}

bool IO::GeoJSON_SAX_Handler::EndObject(rapidjson::SizeType /*elementCount*/)
{
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_OBJECT:
	case Parse_State::FOREIGN_OBJECT:
		Pop_Context();
		return true;
	case Parse_State::GEOMETRY:
		if (auto geometry = Create_Geometry())
		{
			Pop_Context();
			Current_Context().geometries.emplace_back(std::move(*geometry));
			return true;
		}
		return false;
	case Parse_State::FEATURE:
		if (auto feature = Create_Feature())
		{
			Pop_Context();
			m_features.emplace_back(std::move(*feature));
			return true;
		}
		return false;
	case Parse_State::ROOT:
		switch (Current_Context().type)
		{
		case GeoJSON::Type::FEATURE_COLLECTION:
			m_geojson.object = GeoJSON::Feature_Collection{std::move(m_features)};
			m_geojson.bbox = std::move(Current_Context().bbox);
			m_geojson.id = std::move(m_id);
			return true;
		case GeoJSON::Type::FEATURE:
			if (auto feature = Create_Feature())
			{
				m_geojson.object = std::move(*feature);
				m_geojson.bbox = std::move(Current_Context().bbox);
				m_geojson.id = std::move(m_id);
			}
			return true;
		case GeoJSON::Type::POINT:
		case GeoJSON::Type::MULTI_POINT:
		case GeoJSON::Type::LINE_STRING:
		case GeoJSON::Type::MULTI_LINE_STRING:
		case GeoJSON::Type::POLYGON:
		case GeoJSON::Type::MULTI_POLYGON:
		case GeoJSON::Type::GEOMETRY_COLLECTION:
			if (auto geometry = Create_Geometry())
			{
				m_geojson.object = std::move(*geometry);
				m_geojson.bbox = std::move(Current_Context().bbox);
				return true;
			}
			return false;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_STATE_KEY);
	}
	return Push_Error(IO::Error::Type::UNEXPECTED_STATE_OBJECT);
}

bool IO::GeoJSON_SAX_Handler::StartArray() {
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_ARRAY:
	{
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(GeoJSON::Property::Array());
			return Push_Context(Parse_State::PROPERTIES_SUB_ARRAY, Current_Context().property.get().Get_Array().back());
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	}
	case Parse_State::PROPERTIES_SUB_KEY:
	{
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = GeoJSON::Property::Array();
			Reset_State(Parse_State::PROPERTIES_SUB_ARRAY);
			Current_Context().property = Current_Context().property.get().Get_Object()[Current_Context().key_str];
			return true;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	}
	case Parse_State::FOREIGN_KEY:          return Reset_State(Parse_State::FOREIGN_ARRAY);
	case Parse_State::FOREIGN_ARRAY:        return Push_Context(Parse_State::FOREIGN_ARRAY);
	case Parse_State::FEATURE_COLLECTION:
	case Parse_State::GEOMETRY_COLLECTION:
	case Parse_State::BBOX:                 return true;
	case Parse_State::COORDINATES:
	{
		m_positions.Clear();
		m_level++;
		m_max_level = std::max(m_max_level, m_level);
		return true;
	}
	default: return Push_Error(IO::Error::Type::UNEXPECTED_STATE_ARRAY);
	}
}

bool IO::GeoJSON_SAX_Handler::EndArray(rapidjson::SizeType /*elementCount*/)
{
	switch (Current_State())
	{
		case Parse_State::PROPERTIES_SUB_ARRAY:
		case Parse_State::FOREIGN_ARRAY:
		case Parse_State::FEATURE_COLLECTION:
			Pop_Context();
			return true;
		case Parse_State::GEOMETRY_COLLECTION:
			Current_Context().geometries = std::move(Pop_Context().geometries);
			return true;
		case Parse_State::BBOX:
		{
			if (m_positions.Size() == 4)
			{
				Pop_Context();
				Current_Context().bbox = GeoJSON::Bbox{ std::array<double, 4>()};
				for(auto i : std::views::iota(0,4))
					std::get<std::array<double, 4>>(Current_Context().bbox->coordinates)[i] = m_positions[i];
				return true;
			}
			else if(m_positions.Size() == 6)
			{
				Pop_Context();
				Current_Context().bbox = GeoJSON::Bbox{ std::array<double, 6>() };
				for (auto i : std::views::iota(0, 6))
					std::get<std::array<double, 6>>(Current_Context().bbox->coordinates)[i] = m_positions[i];
				return true;
			}
			return Push_Error(IO::Error::Type::BBOX_SIZE_INCONSISTENT);	
		}
		case Parse_State::COORDINATES:
		{
			if (!Finalize_Coordinates())
				return false;
			m_level--;
			if (m_level == 0)
				Pop_Context();
			return true;
		}
		default: return Push_Error(IO::Error::Type::UNEXPECTED_STATE_ARRAY);
	}
}

bool IO::GeoJSON_SAX_Handler::Key(const char* str, rapidjson::SizeType length, bool /*copy*/)
{
	std::string_view key_str(str, length);
	GeoJSON::Key key = Set_Current_Key(key_str);
	
	auto current = Current_State();
	
	switch (current)
	{
		case Parse_State::FOREIGN_OBJECT: return Push_Context(Parse_State::FOREIGN_KEY);
		case Parse_State::PROPERTIES_OBJECT: return Push_Context(Parse_State::PROPERTIES_SUB_KEY, Current_Context().property, key_str);
		case Parse_State::GEOMETRY:
			switch (key)
			{
				case GeoJSON::Key::TYPE:                return Push_Context(Parse_State::TYPE);
				case GeoJSON::Key::COORDINATES:         return Push_Context(Parse_State::COORDINATES);
				case GeoJSON::Key::BBOX:                return Push_Context(Parse_State::BBOX);
				case GeoJSON::Key::FOREIGN:             return Push_Context(Parse_State::FOREIGN_KEY);
				case GeoJSON::Key::GEOMETRY_COLLECTION: return Push_Context(Parse_State::GEOMETRY_COLLECTION);
			}
			break;
		case Parse_State::FEATURE:
			switch (key)
			{
				case GeoJSON::Key::TYPE:       return Push_Context(Parse_State::TYPE);
				case GeoJSON::Key::GEOMETRY:   return Push_Context(Parse_State::GEOMETRY);
				case GeoJSON::Key::PROPERTIES:
					m_property = GeoJSON::Property::Object();
					return Push_Context(Parse_State::PROPERTIES_OBJECT, m_property);
				case GeoJSON::Key::BBOX:       return Push_Context(Parse_State::BBOX);
				case GeoJSON::Key::FOREIGN:    return Push_Context(Parse_State::FOREIGN_KEY);
				case GeoJSON::Key::ID:         return Push_Context(Parse_State::ID);
			}
			break;
		case Parse_State::ROOT:
			switch (key)
			{
				case GeoJSON::Key::TYPE:                return Push_Context(Parse_State::TYPE);
				case GeoJSON::Key::FEATURES_COLLECTION: return Push_Context(Parse_State::FEATURE_COLLECTION);
				case GeoJSON::Key::PROPERTIES:
					m_property = GeoJSON::Property::Object();
					return Push_Context(Parse_State::PROPERTIES_OBJECT, m_property);
				case GeoJSON::Key::BBOX:                return Push_Context(Parse_State::BBOX);
				case GeoJSON::Key::COORDINATES:         return Push_Context(Parse_State::COORDINATES);
				case GeoJSON::Key::FOREIGN:             return Push_Context(Parse_State::FOREIGN_KEY);
				case GeoJSON::Key::GEOMETRY:            return Push_Context(Parse_State::GEOMETRY);
				case GeoJSON::Key::ID:                  return Push_Context(Parse_State::ID);
				case GeoJSON::Key::GEOMETRY_COLLECTION: return Push_Context(Parse_State::GEOMETRY_COLLECTION);
			}
	}
	return Push_Error(IO::Error::Type::UNEXPECTED_STATE_KEY);
}

bool IO::GeoJSON_SAX_Handler::String(const char* str, rapidjson::SizeType length, bool /*copy*/)
{
	switch (Current_State())
	{		
		case Parse_State::TYPE:
			Pop_Context();
			Current_Context().type = GeoJSON::String_To_Type(std::string_view(str, length));
			if (Current_Context().type == GeoJSON::Type::UNKNOWN)
			{
				Push_Error(IO::Error::Type::UNKNOWN_TYPE);
				return false;
			}
			return true;
		case Parse_State::ID:
			m_id = std::string(str, length);
			break;
		case Parse_State::PROPERTIES_SUB_KEY:
			if (Current_Context().property.get().Is_Object())
			{
				if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
					return Push_Error(IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);
				Current_Context().property.get().Get_Object()[Current_Context().key_str] = std::string(str, length);
				break;
			}
			return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
		case Parse_State::PROPERTIES_SUB_ARRAY:
			if (Current_Context().property.get().Is_Array())
			{
				Current_Context().property.get().Get_Array().emplace_back(std::string(str, length));
				break;
			}
			return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
		case Parse_State::FOREIGN_ARRAY: break;
		case Parse_State::FOREIGN_KEY:   break;
		default:
			return Push_Error(IO::Error::Type::UNEXPECTED_STATE_VALUE);
	}
	if(!In_Array())
		Pop_Context();
	return true;
}

bool IO::GeoJSON_SAX_Handler::Bool(bool value)
{
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = value;
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(value);
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::FOREIGN_ARRAY: break;
	case Parse_State::FOREIGN_KEY:   break;
	default:
		return Push_Error(IO::Error::Type::UNEXPECTED_STATE_VALUE);
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

bool IO::GeoJSON_SAX_Handler::Double(double value)
{
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = value;
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(value);
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::ID:
		m_id = std::to_string(value);
		break;
	case Parse_State::COORDINATES:
	case Parse_State::BBOX:
		if (m_level != m_max_level)
			return Push_Error(IO::Error::Type::INCONSCISTENT_COORDINATE_LEVEL);
		m_positions.Emplace_Back(value);
		break;
	case Parse_State::FOREIGN_ARRAY:
	case Parse_State::FOREIGN_KEY: break;
	default:
		return Double(static_cast<double>(value));
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

// Implement other numeric types to handle coordinates
bool IO::GeoJSON_SAX_Handler::Int(int value) 
{ 
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = value;
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(value);
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::ID:
		m_id = std::to_string(value);
		break;
	default:
		return Double(static_cast<double>(value));
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

bool IO::GeoJSON_SAX_Handler::Uint(unsigned value) 
{ 
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = static_cast<int>(value);
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(static_cast<int>(value));
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::ID:
		m_id = std::to_string(value);
		break;
	default:
		return Double(static_cast<double>(value));
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

bool IO::GeoJSON_SAX_Handler::Int64(int64_t value) 
{ 
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = static_cast<int>(value);
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(static_cast<int>(value));
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::ID:
		m_id = std::to_string(value);
		break;
	default:
		return Double(static_cast<double>(value));
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

bool IO::GeoJSON_SAX_Handler::Uint64(uint64_t value) 
{
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = static_cast<int>(value);
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(static_cast<int>(value));
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::ID:
		m_id = std::to_string(value);
		break;
	default:
		return Double(static_cast<double>(value));
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

bool IO::GeoJSON_SAX_Handler::RawNumber(const char* str, rapidjson::SizeType length, bool /*copy*/) {
	// Convert string to double
	try
	{
		double value = std::stod(std::string(str, length));
		return Double(value);
	} 
	catch (const std::exception&)
	{
		return false;
	}
}

bool IO::GeoJSON_SAX_Handler::Null()
{
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = GeoJSON::Property();
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(GeoJSON::Property());
			break;
		}
		return Push_Error(IO::Error::Type::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_OBJECT:
	case Parse_State::GEOMETRY:
	case Parse_State::FOREIGN_ARRAY:
	case Parse_State::FOREIGN_KEY: break;
	default:
		return Push_Error(IO::Error::Type::UNEXPECTED_STATE_VALUE);
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

bool IO::GeoJSON_SAX_Handler::Finalize_Coordinates()
{
	// verify current pushed positions
	if (m_level != m_max_level)
	{
		m_add_level = m_max_level - m_level;
		if (m_positions.Size()) return Push_Error(IO::Error::Type::INCONSCISTENT_COORDINATE_LEVEL);
		return true;
	}
	else if (m_positions.Size() < 2) return Push_Error(IO::Error::Type::COORDINATE_UNDERSIZED);
	else if (m_positions.Size() > 3) return Push_Error(IO::Error::Type::COORDINATE_OVERSIZED);
	
	auto position = Level1{ m_positions[0], m_positions[1], (m_positions.Size() == 3) ? std::optional<double>(m_positions[2]) : std::nullopt };
	m_positions.Clear();
	switch(m_max_level)
	{
		case 1:
			m_coordinate = std::move(position);
			return true;
		case 2:
		{
			if (!std::holds_alternative<Level2>(m_coordinate)) { m_coordinate = Level2{}; };
			Level2& obj = std::get<Level2>(m_coordinate);
			obj.emplace_back(std::move(position));
			return true;
		}
		case 3:
		{
			if (!std::holds_alternative<Level3>(m_coordinate)) { m_coordinate = Level3{ Level2{} }; };
			Level3& obj = std::get<Level3>(m_coordinate);
			if (m_add_level == 0) {}
			else if (m_add_level == 1)
			{
				obj.emplace_back(Level2{});
				m_add_level = false;
			}
			else
				return Push_Error(IO::Error::Type::INCONSCISTENT_COORDINATE_LEVEL);
			obj.back().emplace_back(std::move(position));
			return true;
		}
		case 4:
		{
			if (!std::holds_alternative<Level4>(m_coordinate)) { m_coordinate = Level4{ GeoJSON::Polygon{Level3{Level2{}}} }; };
			Level4& obj = std::get<Level4>(m_coordinate);
			if (m_add_level == 0) {}
			else if (m_add_level == 1)
			{
				obj.back().rings.emplace_back(Level2{});
				m_add_level = false;
			}
			else if (m_add_level == 2)
			{
				obj.emplace_back(GeoJSON::Polygon{ Level3{Level2{}} });
				m_add_level = false;
			}
			else
				return Push_Error(IO::Error::Type::INCONSCISTENT_COORDINATE_LEVEL);
			obj.back().rings.back().emplace_back(std::move(position));
			return true;
		}
		default: return false;
	}
}

std::optional<GeoJSON::Feature> IO::GeoJSON_SAX_Handler::Create_Feature()
{
	if(!Current_Context().geometries.empty())
	{
		if(auto bbox = Current_Context().bbox)
		{
			if(auto& id = m_id)
				return GeoJSON::Feature{std::move(m_context_stack.back().geometries[0]),std::move(m_property), std::move(*id), std::move(*bbox)};
			else
				return GeoJSON::Feature{std::move(m_context_stack.back().geometries[0]),std::move(m_property), std::nullopt, std::move(*bbox)};
		}
		else if(auto& id = m_id)
			return GeoJSON::Feature{std::move(m_context_stack.back().geometries[0]),std::move(m_property), std::move(*id), std::nullopt};
		else
			return GeoJSON::Feature{std::move(m_context_stack.back().geometries[0]), std::move(m_property), std::nullopt, std::nullopt};
	}
	Push_Error(IO::Error::Type::FEATURE_NEED_INITIALIZED_GEOMETRY);
	return std::nullopt;
}

std::optional<GeoJSON::Geometry> IO::GeoJSON_SAX_Handler::Create_Geometry()
{
	auto Fail = [&](IO::Error::Type e) -> std::optional<GeoJSON::Geometry>
	{
		Push_Error(e);
		return std::nullopt;
	};


	auto Make_Multi_Line_String = [](Level3 & coordinates) -> GeoJSON::Multi_Line_String
	{
		std::vector<GeoJSON::Line_String> lines;
		for (auto&& coordiante : coordinates)
			lines.emplace_back(GeoJSON::Line_String{ std::move(coordiante) });
		return GeoJSON::Multi_Line_String{ std::move(lines) };
	};

	auto Make_Multi_Polygon = [](Level4& coordinates) -> GeoJSON::Multi_Polygon
	{
		std::vector<GeoJSON::Polygon> polygones;
		for (auto&& coordiante : coordinates)
			polygones.emplace_back(GeoJSON::Polygon{ std::move(coordiante) });
		return GeoJSON::Multi_Polygon{ std::move(polygones) };
	};

	switch (Current_Context().type)
	{
		case GeoJSON::Type::POINT:
			if (!std::holds_alternative<Level1>(m_coordinate))
				return Fail(IO::Error::Type::BAD_COORDINATE_FOR_GEMETRY);
			return GeoJSON::Geometry{ GeoJSON::Point{std::move(std::get<Level1>(m_coordinate))}, std::move(Current_Context().bbox)};

		case GeoJSON::Type::MULTI_POINT:
			if (!std::holds_alternative<Level2>(m_coordinate))
				return Fail(IO::Error::Type::BAD_COORDINATE_FOR_GEMETRY);
			return GeoJSON::Geometry{ GeoJSON::Multi_Point{std::move(std::get<Level2>(m_coordinate))}, std::move(Current_Context().bbox) };

		case GeoJSON::Type::LINE_STRING:
			if (!std::holds_alternative<Level2>(m_coordinate))
				return Fail(IO::Error::Type::BAD_COORDINATE_FOR_GEMETRY);
			if (std::get<Level2>(m_coordinate).size() < 2)
				return Fail(IO::Error::Type::NEED_AT_LEAST_TWO_POSITION_FOR_LINESTRING);
			return GeoJSON::Geometry{ GeoJSON::Line_String{std::move(std::get<Level2>(m_coordinate))}, std::move(Current_Context().bbox) };

		case GeoJSON::Type::MULTI_LINE_STRING:
			if (!std::holds_alternative<Level3>(m_coordinate))
				return Fail(IO::Error::Type::BAD_COORDINATE_FOR_GEMETRY);
			for(auto& line_string : std::get<Level3>(m_coordinate))
				if(line_string.size() < 2)
					return Fail(IO::Error::Type::NEED_AT_LEAST_TWO_POSITION_FOR_LINESTRING);
			return GeoJSON::Geometry{ Make_Multi_Line_String(std::get<Level3>(m_coordinate)), std::move(Current_Context().bbox) };

		case GeoJSON::Type::POLYGON:
			if (!std::holds_alternative<Level3>(m_coordinate))
				return Fail(IO::Error::Type::BAD_COORDINATE_FOR_GEMETRY);
			if (std::get<Level3>(m_coordinate).empty())
				return Fail(IO::Error::Type::POLYGON_NEED_AT_LEAST_ONE_RING);
			for (auto& ring : std::get<Level3>(m_coordinate))
			{
				if (ring.size() < 4)
					return Fail(IO::Error::Type::NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON);
				if (ring.front().altitude != ring.back().altitude || ring.front().latitude != ring.back().latitude || ring.front().longitude != ring.back().longitude)
					return Fail(IO::Error::Type::POLYGON_NEED_TO_BE_CLOSED);
			}
			return GeoJSON::Geometry{ GeoJSON::Polygon(std::get<Level3>(m_coordinate)), std::move(Current_Context().bbox) };

		case GeoJSON::Type::MULTI_POLYGON:
			if (!std::holds_alternative<Level4>(m_coordinate))
				return Fail(IO::Error::Type::BAD_COORDINATE_FOR_GEMETRY);
			for (auto& polygone : std::get<Level4>(m_coordinate))
			{
				if(polygone.rings.empty())
					return Fail(IO::Error::Type::POLYGON_NEED_AT_LEAST_ONE_RING);
				for (auto& ring : polygone.rings)
				{
					if (ring.size() < 4)
						return Fail(IO::Error::Type::NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON);
					if (ring.front().altitude != ring.back().altitude || ring.front().latitude != ring.back().latitude || ring.front().longitude != ring.back().longitude)
						return Fail(IO::Error::Type::POLYGON_NEED_TO_BE_CLOSED);
				}
			}
			return GeoJSON::Geometry{ Make_Multi_Polygon(std::get<Level4>(m_coordinate)) , std::move(Current_Context().bbox) };

		case GeoJSON::Type::GEOMETRY_COLLECTION:
		{
			GeoJSON::Geometry_Collection collection;
			for (auto& geometry : m_context_stack.back().geometries)
				collection.geometries.emplace_back(std::make_shared<GeoJSON::Geometry>(geometry));
			return GeoJSON::Geometry{ std::move(collection), std::move(Current_Context().bbox) };
		}
		default:
			Push_Error(IO::Error::Type::UNKNOWN_GEOMETRY_TYPE);
			return std::nullopt;
	}
}