#ifndef IO_FEATURE_PARSER_H
#define IO_FEATURE_PARSER_H

#include "sax_parser.h"

namespace O::GeoJSON::IO
{
	/**
	 * @brief CRTP-based base class for handling GeoJSON Feature-related SAX events.
	 *        This class defines callback hooks invoked by SAX_Parser when specific GeoJSON objects are fully parsed:
	 *          - Geometry objects
	 *          - Feature objects
	 *          - FeatureCollection objects
	 *          - Root entry point with optional bbox/id
	 * Derived classes can override these methods to implement custom business logic (indexing, validation, transformation, storage, etc).
	 * @tparam Derived The CRTP derived class that implements the handlers.
	 */
	template <class Derived>
	class Feature_Parser : public SAX_Parser<Feature_Parser<Derived>>
	{
	public:

		/**
		 * @brief Called when a complete Feature has been parsed.
		 *        This is the main entry point when parsing a standalone Feature, or when encountering a Feature inside a FeatureCollection.
		 * @param feature Fully materialized GeoJSON Feature.
		 * @return true if parsing should continue, false to abort.
		 */
		bool On_Full_Feature(O::GeoJSON::Feature&& feature);

		/**
		 * @brief Called when the root-level metadata (bbox, id) is parsed.
		 *        This is invoked for both Feature and FeatureCollection root objects.
		 * @param bbox Optional bounding box.
		 * @param id Optional identifier string.
		 * @return true if parsing should continue, false to abort.
		 */
		bool On_Root(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);
		
		/// @name CRTP implementation
		/// @brief Implementation of the Base SAX parser
		/// @{
		bool On_Geometry(O::GeoJSON::Geometry&& geometry, std::size_t element_number);
		bool On_Feature(O::GeoJSON::Feature&& feature);
		bool On_Feature_Collection(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);
		/// @}

	private:
		std::vector<O::GeoJSON::Geometry> m_geometries; ///< Temporary geometry accumulator used while parsing.
	};
}

#include "feature_parser.hpp"

#endif // IO_FEATURE_PARSER_H