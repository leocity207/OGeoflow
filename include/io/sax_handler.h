#ifndef IO_SAX_HANDLER_H
#define IO_SAX_HANDLER_H

#include <variant>
#include <rapidjson/reader.h>

#include "include/geojson/object/geometry.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/geometry_type/polygon.h"
#include "include/geojson/position.h"
#include "include/geojson/bbox.h"
#include "include/geojson/geojson.h"
#include "include/io/error.h"
#include "include/utils/bounded_array.h"

namespace IO
{
	inline GeoJSON::Property PROPERTY_STUB;

	class GeoJSON_SAX_Handler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, GeoJSON_SAX_Handler> 
	{
	// HELPER
	public:
		// Parsing state
		enum class Parse_State {
			TYPE,
			ROOT,
			FEATURE_COLLECTION,
			FEATURE,
			GEOMETRY,
			GEOMETRY_COLLECTION,
			COORDINATES,
			BBOX,
			ID,
			PROPERTIES,
			PROPERTIES_OBJECT,
			PROPERTIES_SUB_ARRAY,
			PROPERTIES_SUB_KEY,
			FOREIGN_KEY,
			FOREIGN_ARRAY,
			FOREIGN_OBJECT,
			UNKNOWN
		};

		struct Parse_Context
		{
			std::reference_wrapper<GeoJSON::Property> property;
			Parse_State state;
			std::string key_str;
			GeoJSON::Key key;
			GeoJSON::Type type; 	
			std::vector<GeoJSON::Geometry> geometries;
			std::optional<GeoJSON::Bbox> bbox;
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
		bool RawNumber(const char* str, rapidjson::SizeType length, bool copy);

		/**
		 * @brief Get the parsed GeoJSON object
		 * @return Parsed GeoJSON structure
		 */
		GeoJSON::GeoJSON&& Get_Geojson() {return std::move(m_geojson);};
		Error::Type Get_Error() const {return m_current_error;}

	private:

		// Helper methods to help handle the context we are in within the sax handler
		bool Push_Context(Parse_State state, GeoJSON::Property& ref_property = PROPERTY_STUB, std::string_view key = "");
		bool Reset_State(Parse_State state);
		Parse_Context Pop_Context();
		Parse_State Current_State() const;
		GeoJSON::Key Set_Current_Key(std::string_view key);
		Parse_Context& Current_Context();
		bool In_Array();
		

		// Create object from the given parsed data
		std::optional<GeoJSON::Feature> Create_Feature();
		std::optional<GeoJSON::Geometry> Create_Geometry();
		
		// Coordinate processing
		bool Process_Coordinate_Number(double value);
		bool Finalize_Coordinates();
		bool Push_Error(Error::Type error) { m_current_error = error; return false; };

	// ATTRIBUTE
	private:
		std::vector<Parse_Context> m_context_stack;
		GeoJSON::Property m_property;
		Util::Bounded_Vector<double,6> m_positions;
		GeoJSON::GeoJSON m_geojson;
		std::optional<std::string> m_id;
		std::vector<GeoJSON::Feature> m_features;
		std::variant<
			GeoJSON::Position, //level 1
			std::vector<GeoJSON::Position>, //level 2
			std::vector<std::vector<GeoJSON::Position>>, //level 3
			std::vector<GeoJSON::Polygon> //// level 4
		> m_coordinate;

		char m_level;
		char m_max_level;
		char m_add_level = 0;
		Error::Type m_current_error;	
	};
} // namespace GeoJSON

#endif //IO_SAX_HANDLER_H