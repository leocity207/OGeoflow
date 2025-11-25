#ifndef GEOJSON_GEOMETRY_POINT_H
#define GEOJSON_GEOMETRY_POINT_H

#include <vector>

#include "include/geojson/position.h"

namespace O::GeoJSON
{
	/**
	 * @brief Point geometry - hold a single position
	 */
	struct Point
	{
		Position position;
	};
}

#endif //GEOJSON_GEOMETRY_POINT_H