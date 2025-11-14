#ifndef IO_FEATURE_PARSER_H
#define IO_FEATURE_PARSER_H

/**
 * @file feature_parser.h
 * @brief Definition of Feature_Parser, a SAX-based GeoJSON feature parsing helper.
 *
 * This class provides callback entry points for processing GeoJSON Features,
 * FeatureCollections, and embedded geometries. It is designed to be used by
 * inheriting from the CRTP template:
 *
 * @code
 * class MyParser : public Feature_Parser<MyParser>
 * {
 *     bool On_Full_Feature(::GeoJSON::Feature&& feature)
 *     {
 *         // custom logic
 *         return true;
 *     }
 * };
 * @endcode
 *
 * Feature_Parser is part of the GeoJSON::IO namespace and works together with
 * SAX_Parser to consume streaming JSON input efficiently.
 */

#include "sax_parser.h"

namespace O::GeoJSON::IO
{
    /**
     * @class Feature_Parser
     * @brief CRTP-based base class for handling GeoJSON Feature-related SAX events.
     *
     * This class defines callback hooks invoked by SAX_Parser when
     * specific GeoJSON objects are fully parsed:
     *
     * - Geometry objects
     * - Feature objects
     * - FeatureCollection objects
     * - Root entry point with optional bbox/id
     *
     * Derived classes can override these methods to implement custom
     * business logic (indexing, validation, transformation, storage, etc).
     *
     * @tparam Derived The CRTP derived class that implements the handlers.
     */
    template <class Derived>
    class Feature_Parser : public SAX_Parser<Feature_Parser<Derived>>
    {
    public:

        /**
         * @brief Called when a complete Feature has been parsed.
         *
         * This is the main entry point when parsing a standalone Feature,
         * or when encountering a Feature inside a FeatureCollection.
         *
         * @param feature Fully materialized GeoJSON Feature.
         * @return true if parsing should continue, false to abort.
         */
        bool On_Full_Feature(O::GeoJSON::Feature&& feature);

        /**
         * @brief Called when the root-level metadata (bbox, id) is parsed.
         *
         * This is invoked for both Feature and FeatureCollection root objects.
         *
         * @param bbox Optional bounding box.
         * @param id Optional identifier string.
         * @return true if parsing should continue, false to abort.
         */
        bool On_Root(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);

        /**
         * @brief Called when a geometry object belonging to a Feature is parsed.
         *
         * This callback allows derived classes to handle individual geometries
         * encountered inside a Feature.
         *
         * @param geometry The parsed geometry.
         * @param element_number Index of the geometry within the feature.
         * @return true if parsing should continue, false to abort.
         */
        bool On_Geometry(O::GeoJSON::Geometry&& geometry, std::size_t element_number);

        /**
         * @brief Called after a Feature subobject is parsed (not necessarily root).
         *
         * Useful for nested/recursive structures or when processing
         * an array of features within a FeatureCollection.
         *
         * @param feature Parsed feature object.
         * @return true if parsing should continue, false to abort.
         */
        bool On_Feature(O::GeoJSON::Feature&& feature);

        /**
         * @brief Called when a FeatureCollection is parsed.
         *
         * This callback provides access to the collection-level bbox and id attributes.
         * Individual features inside the collection trigger On_Feature / On_Full_Feature.
         *
         * @param bbox Optional bounding box.
         * @param id Optional collection ID.
         * @return true if parsing should continue, false to abort.
         */
        bool On_Feature_Collection(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);

    private:
        /// Temporary geometry accumulator used while parsing.
        std::vector<O::GeoJSON::Geometry> m_geometries;

        /// True if a FeatureCollection is being parsed.
        bool m_is_feature_collection = false;

        /// Internal flag used to detect parsing validity or structural errors.
        bool m_valid = true;
    };
}

#include "feature_parser.hpp"

#endif // IO_FEATURE_PARSER_H