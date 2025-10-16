#ifndef GEOJSON_FEATURE_H
#define GEOJSON_FEATURE_H

#include "geometry.h"
#include "json_value.h"

namespace GeoJSON
{

	/**
	 * @brief Feature object containing geometry and properties
	 */
	struct Feature
	{
		std::unique_ptr<Geometry> geometry;
		JSON_Object properties;
		std::string id;  // Optional identifier
	};
}

#endif //GEOJSON_FEATURE_H