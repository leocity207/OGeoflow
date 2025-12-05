#ifndef IO_SAX_HANDLER_H
#define IO_SAX_HANDLER_H

#include <variant>
#include <rapidjson/reader.h>

// GeoJSON
#include "geojson/object/geometry.h"
#include "geojson/object/feature.h"
#include "geojson/geometry_type/polygon.h"
#include "geojson/position.h"
#include "geojson/bbox.h"
#include "geojson/root.h"

// IO
#include "io/error.h"

// UTILS
#include "utils/bounded_array.h"

namespace O::GeoJSON::IO
{
	/**
	 * @brief Stub property placeholder used when no property context is required.
	 */
	inline O::GeoJSON::Property PROPERTY_STUB;

	/**
	 * @class SAX_Parser
	 * @tparam Derived Optional CRTP parameter used when extending parser behavior.
	 * @brief Streaming SAX parser for GeoJSON documents.
	 *        This class consumes JSON through RapidJSON's SAX interface and reconstructs GeoJSON objects incrementally. 
	 *        It maintains an internal parsing stack representing the current context within the GeoJSON structure (feature, geometry, coordinates, properties, etc.).
	 *        A user who wants to process GeoJSON elements must subclass this parser and implement the following callback methods:
	 *          - **On_Geometry()**  Invoked whenever a standalone `Geometry` object is fully parsed.
	 *          - **On_Feature()**   Invoked when a GeoJSON `Feature` object is complete.
	 *          - **On_Feature_Collection()**   Invoked at the end of a `FeatureCollection`, after all contained features have been streamed through `On_Feature()`.
	 * These callbacks allow the user to handle GeoJSON elements as they stream in, without requiring the entire document to be stored in memory.
	 */
	template<class Derived = void>
	class SAX_Parser : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, SAX_Parser<Derived>>
	{
	public:
		/**
		 * @brief Callback invoked when a complete Geometry has been parsed.
		 * @param geometry Parsed geometry object (rvalue reference for efficiency).
		 * @param element_number Index of the geometry if part of a collection.
		 * @return `true` to continue parsing, `false` to abort.
		 */
		bool On_Geometry(O::GeoJSON::Geometry&& geometry, std::size_t element_number);

		/**
		 * @brief Callback invoked when a complete Feature has been parsed.
		 * @param feature Parsed GeoJSON Feature.
		 * @return `true` to continue parsing, `false` to abort.
		 */
		bool On_Feature(O::GeoJSON::Feature&& feature);

		/**
		 * @brief Callback invoked at the end of a FeatureCollection.
		 * @param bbox Optional bounding box for the entire collection.
		 * @param id Optional identifier for the FeatureCollection.
		 * @return `true` to continue parsing, `false` to abort.
		 * @note Features in the collection are streamed individually through `On_Feature()` before this function is called.
		 */
		bool On_Feature_Collection(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);

		/**
		 * @brief Parsing state machine used to interpret JSON tokens.
		 *        Each state represents a specific context inside the GeoJSON structure.
		 */
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

		/**
		 * @brief Stack element storing the current parsing context.
		 *        This structure tracks:
		 *          - the current `Parse_State`
		 *          - the last key encountered
		 *          - the inferred GeoJSON type
		 *          - optional in-progress Geometry
		 *          - optional bounding box
		 *          - reference to a property structure when parsing Feature properties
		 */
		struct Parse_Context
		{
			std::reference_wrapper<O::GeoJSON::Property> property;
			Parse_State state;
			std::string key_str;
			O::GeoJSON::Key key;
			O::GeoJSON::Type type; 	
			std::optional<O::GeoJSON::Geometry> geometry;
			std::size_t geometry_count;
			std::optional<O::GeoJSON::Bbox> bbox;
		};

		SAX_Parser();
		~SAX_Parser() = default;

		/// @name RapidJSON Event Overrides
		/// @brief These functions map incoming JSON tokens to internal state.
		/// @{
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
		/// @}

		/**
		 * @brief Get the last recorded parsing error.
		 * @return One of the error codes defined in `GeoJSON::Error::Type`.
		 */
		O::GeoJSON::IO::Error Get_Error() const { return m_current_error; }

	protected:

		/**
		 * @brief Record an error and stop parsing.
		 * @param error Error code describing the failure.
		 * @return Always returns false to abort RapidJSON parsing.
		 */
		bool Push_Error(Error error);

	private:

		/** 
		* @brief Push a new context to the context stack
		* @param state the new state
		* @param ref_property property reference used to construct properties
		* @param key the key when you are in an object
		* @return wethere or not it went well
		*/
		bool Push_Context(Parse_State state, O::GeoJSON::Property& ref_property = PROPERTY_STUB, std::string_view key = "");

		/// @brief reset the curent state to the given ```state``
		bool Reset_State(Parse_State state);

		/// @brief Pop the current context and give it back
		Parse_Context Pop_Context();

		/// @brief give back the current context state
		Parse_State Current_State() const;

		/// @brief Set the current key inside the context
		O::GeoJSON::Key Set_Current_Key(std::string_view key);

		/// @brief get a reference to the current context
		Parse_Context& Current_Context();

		/// @brief tells if we are parsing an array or not
		bool In_Array();

		/// @brief Factory function that create a Feature from the current state
		std::optional<O::GeoJSON::Feature> Create_Feature();

		/// @brief Factory function that create a Geometry from the current state
		std::optional<O::GeoJSON::Geometry> Create_Geometry();

		/// @brief process the current coordinates number inside the current coordinate level
		bool Finalize_Coordinates();

		std::vector<Parse_Context> m_context_stack; ///< Context Stack
		O::GeoJSON::Property m_property;            ///< current property inside the current feature
		O::Bounded_Vector<double,6> m_positions;    ///< Temporary position buffer used to accumulate coordinate tuples.
		std::optional<std::string> m_id;            ///< Current id inside the Feature
		char m_level;                               ///< current level of coordinate
		char m_max_level;                           ///< max reach level of coordinate
		char m_add_level = 0;                       ///< difference of level between max and level at the previous step
		Error m_current_error;                      ///< Current registered error

		/**
		 * @brief Coordinate accumulator used to construct Position, LineString, Polygon, or MultiPolygon structures depending on nesting level.
		 *        Level interpretation:
		 *          - level 1 : Position
		 *          - level 2 : vector<Position>
		 *          - level 3 : vector<vector<Position>>
		 *          - level 4 : vector<Polygon>
		 */
		std::variant<
			O::GeoJSON::Position,
			std::vector<O::GeoJSON::Position>,
			std::vector<std::vector<O::GeoJSON::Position>>,
			std::vector<O::GeoJSON::Polygon>
		> m_coordinate;
	};

} // namespace O::GeoJSON::IO

#include "sax_parser.hpp"


#endif //IO_SAX_HANDLER_H