#ifndef GEOJSON_GEOMETRY_MULTI_POLYGON_H
#define GEOJSON_GEOMETRY_MULTI_POLYGON_H

#include <vector>

#include "polygon.h"

namespace O::GeoJSON
{

	/**
	 * @brief MultiPolygon geometry - array of Polygon geometries
	 */
	struct Multi_Polygon
	{
		std::vector<Polygon> polygons;
	};
}

#endif //GEOJSON_GEOMETRY_MULTI_POLYGON_H