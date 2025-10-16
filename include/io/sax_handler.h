#ifndef IO_SAX_HANDLER_H
#define IO_SAX_HANDLER_H


#include <rapidjson/reader.h>

#include "include/geojson/geojson.h"
#include "include/geojson/json_value.h"
#include "include/geojson/position.h"

namespace IO
{

	class GeoJSON_SAX_Handler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, GeoJSON_SAX_Handler> 
	{
	// HELPER
	private:
		// Parsing state
		enum class Parse_State {
			ROOT,
			FEATURE_COLLECTION,
			FEATURE,
			GEOMETRY,
			GEOMETRY_COLLECTION,
			PROPERTIES,
			COORDINATES,
			BBOX,
			UNKNOWN
		};

		struct Parse_Context {
			Parse_State state;
			std::string current_key;
			GeoJSON::JSON_Value current_value;
		};
	// CTOR
	public:
		GeoJSON_SAX_Handler();
		~GeoJSON_SAX_Handler() = default;

	// METHODS
	public:
		// RapidJSON SAX methods
		bool StartObject();
		bool EndObject(rapidjson::SizeType elementCount);
		bool StartArray();
		bool EndArray(rapidjson::SizeType elementCount);
		bool Key(const char* str, rapidjson::SizeType length, bool copy);
		bool String(const char* str, rapidjson::SizeType length, bool copy);
		bool Bool(bool value);
		bool Int(int value);
		bool Uint(unsigned value);
		bool Int64(int64_t value);
		bool Uint64(uint64_t value);
		bool Double(double value);
		bool Null();
		bool RawNumber(const char* str, rapidjson::SizeType length,bool copy);

		/**
		 * @brief Get the parsed GeoJSON object
		 * @return Parsed GeoJSON structure
		 */
		GeoJSON::GeoJSON&& Get_Geojson();

		/**
		 * @brief Check if parsing was successful
		 * @return true if parsing completed successfully
		 */
		bool Is_Valid() const;
	private:

		// Helper methods to help handle the context we are in within the sax handler
		void Push_Context(Parse_State state);
		void Pop_Context();
		Parse_State Current_Context() const;
		void Set_Current_Key(const std::string& key);
		
		// Geometry parsing helpers
		void Parse_Position(const GeoJSON::JSON_Array& arr);
		void Parse_Multi_Point_Coordinates(const GeoJSON::JSON_Array& arr);
		void Parse_Line_String_Coordinates(const GeoJSON::JSON_Array& arr);
		void Parse_Multi_Line_string_Coordinates(const GeoJSON::JSON_Array& arr);
		void Parse_Multi_Polygon_Coordinates(const GeoJSON::JSON_Array& arr);
		

		// Create object from the given parsed data
		std::unique_ptr<GeoJSON::Point> Create_Point();
		std::unique_ptr<GeoJSON::Multi_Point> Create_Multi_Point();
		std::unique_ptr<GeoJSON::Line_String> Create_Line_String();
		std::unique_ptr<GeoJSON::Multi_Line_String> Create_Multi_Line_String();
		std::unique_ptr<GeoJSON::Polygon> Create_Polygon();
		std::unique_ptr<GeoJSON::Multi_Polygon> Create_Multi_Polygon();
		std::unique_ptr<GeoJSON::Geometry_Collection> Create_Geometry_Collection(const GeoJSON::JSON_Array& geometries_array);
		std::unique_ptr<GeoJSON::Geometry_Collection> Parse_Geometry_Collection_Coordinates(const std::string& geometry_type, const GeoJSON::JSON_Array& coordinates_array);
		std::unique_ptr<GeoJSON::Geometry> Parse_Geometry_Coordinates(const std::string& geometry_type, const GeoJSON::JSON_Array& coordinates_array);
		std::unique_ptr<GeoJSON::Point> Parse_Point_Coordinates(const GeoJSON::JSON_Array& coordinates);
		std::unique_ptr<GeoJSON::Multi_Point> Parse_MultiPoint_Coordinates(const GeoJSON::JSON_Array& coordinates);
		std::unique_ptr<GeoJSON::Line_String> Parse_LineString_Coordinates(const GeoJSON::JSON_Array& coordinates);
		std::unique_ptr<GeoJSON::Multi_Line_String> Parse_MultiLineString_Coordinates(const GeoJSON::JSON_Array& coordinates);
		std::unique_ptr<GeoJSON::Polygon> Parse_Polygon_Coordinates(const GeoJSON::JSON_Array& coordinates);
		std::unique_ptr<GeoJSON::Multi_Polygon> Parse_MultiPolygon_Coordinates(const GeoJSON::JSON_Array& coordinates);
			
		// Validation methods
		void Validate_Geojson_Type(const std::string& type);
		void Validate_Geometry_Type(const std::string& type);
		GeoJSON::Geometry_Type String_To_Geometry_Type(const std::string& type_str);

		// Coordinate processing
		void Process_Coordinate_Number(double value);
		void Finalize_Coordinates();

	// ATTRIBUTE
	private:
		GeoJSON::GeoJSON m_geojson;
		std::vector<Parse_Context> m_context_stack;
		std::vector<GeoJSON::Position> m_coordinate_buffer;
		std::vector<double> m_number_buffer;
		std::vector<std::vector<GeoJSON::Position>> m_ring_buffer; 
    	std::vector<std::vector<std::vector<GeoJSON::Position>>> m_polygon_buffer;

		bool m_parsing_successful;
		bool m_in_coordinates;
		size_t m_coordinate_depth;
		std::string m_current_geometry_type;
	};
} // namespace GeoJSON

#endif //IO_SAX_HANDLER_H