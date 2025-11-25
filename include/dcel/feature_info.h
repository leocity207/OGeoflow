#ifndef DCEL_FEATURE_INFO_H
#define DCEL_FEATURE_INFO_H

#include <vector>
#include <optional>
#include <string>

#include "include/geojson/properties.h"
#include "include/geojson/bbox.h"

namespace O::DCEL
{
    struct Feature_Info
    {
        std::vector<O::GeoJSON::Property> feature_properties;
        std::vector<std::optional<std::string>> ids;
        std::vector<std::optional<O::GeoJSON::Bbox>> bboxes;

        bool has_root = false;
        std::optional<std::string> root_id;
        std::optional<O::GeoJSON::Bbox> root_bbox;
    };
}

#endif // DCEL_FEATURE_INFO_H
