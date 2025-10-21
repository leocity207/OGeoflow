#ifndef GEOJSON_FEATURE_H
#define GEOJSON_FEATURE_H

#include <variant>
#include "geometry.h"
#include "include/geojson/properties.h"
#include "include/geojson/bbox.h"

namespace GeoJSON
{

	/**
	 * @brief Feature object containing geometry and properties
	 */
	struct Feature_Collection;
	struct Feature
	{
		Geometry geometry;
		Property properties;
		std::optional<std::string> id;
		std::optional<Bbox> bbox;
	};
}

#endif //GEOJSON_FEATURE_H