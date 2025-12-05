#ifndef DCEL_FEATURE_INFO_H
#define DCEL_FEATURE_INFO_H

#include <vector>
#include <optional>
#include <string>

#include "geojson/properties.h"
#include "geojson/bbox.h"

namespace O::DCEL
{
    /**
     * @brief Feature_Info is a structure that help retain GeoJSON data after the construction of the DCEL for later GeoJSON reconstruction
     * @note all feature encountered inside the GeoJSON are push with their respective index inside each lists. ``feature_properties``, ``ids``, ``bboxes`` are assumed to be of the same size
     */
    struct Feature_Info
    {
        std::vector<O::GeoJSON::Property> feature_properties; ///< list of properties
        std::vector<std::optional<std::string>> ids;          ///< list of id for all the features
        std::vector<std::optional<O::GeoJSON::Bbox>> bboxes;  ///< list of all bbox for each feature

        bool has_root = false;                                ///< if we had a root while parsing the GeoJSON
        std::optional<std::string> root_id;                   ///< root id value in the GeoJSON
        std::optional<O::GeoJSON::Bbox> root_bbox;            ///< root bbox value in the GeoJSON
    };
}

#endif // DCEL_FEATURE_INFO_H
