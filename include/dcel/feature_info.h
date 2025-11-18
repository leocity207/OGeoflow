#ifndef DCEL_FEATURE_INFO_H
#define DCEL_FEATURE_INFO_H

#include <vector>
#include <optional>
#include <string>

#include "include/geojson/properties.h"
#include "include/geojson/bbox.h"

namespace DCEL
{
    struct Feature_Info
    {
        std::vector<GeoJSON::Property> feature_properties;
        std::vector<std::optional<std::string>> ids;
        std::vector<std::optional<GeoJSON::Bbox>> bboxes;

        bool has_root = false;
        std::optional<std::string> root_id;
        std::optional<GeoJSON::Bbox> root_bbox;
    };
}

#endif // DCEL_FEATURE_INFO_H
