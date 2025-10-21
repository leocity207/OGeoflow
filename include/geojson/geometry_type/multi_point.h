#ifndef GEOJSON_GEOMETRY_MULTI_POINT_H
#define GEOJSON_GEOMETRY_MULTI_POINT_H

#include <vector>

#include "include/geojson/position.h"

namespace GeoJSON
{

	/**
	 * @brief MultiPoint geometry - array of positions
	 */
	struct Multi_Point
	{
		std::vector<Position> points;
	};
}

#endif //GEOJSON_GEOMETRY_MULTI_POINT_H