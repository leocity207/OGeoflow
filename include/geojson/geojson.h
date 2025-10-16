#ifndef GEOJSON_GEOJSON_H
#define GEOJSON_GEOJSON_H

#include <optional>

#include "json_value.h"
#include "feature.h"

namespace GeoJSON
{

	/**
	 * @brief Main GeoJSON container
	 */
	struct GeoJSON {
		enum Type
		{
			FEATURE_COLLECTION,
			FEATURE,
			GEOMETRY,
		};

		Type type = FEATURE_COLLECTION;
		std::vector<Feature> features;
		std::unique_ptr<Geometry> geometry;  // For when root is a single geometry
		JSON_Object properties;  // For when root is a Feature
		
		// Bounding box (optional)
		std::optional<std::vector<double>> bbox;
	};
}

#endif //GEOJSON_GEOJSON_H