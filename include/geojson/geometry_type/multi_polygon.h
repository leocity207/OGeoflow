#ifndef GEOJSON_GEOMETRY_MULTI_POLYGON_H
#define GEOJSON_GEOMETRY_MULTI_POLYGON_H

#include <vector>
#include "polygon.h"

namespace O::GeoJSON
{
	/**
	 * @brief Represents a GeoJSON MultiPolygon geometry.
	 *        A MultiPolygon contains multiple Polygon geometries.
	 *
	 * Example JSON:
	 * @code
	 * {
	 *   "type": "MultiPolygon",
	 *   "coordinates": [
	 *       [[[102.0, 2.0], [103.0, 2.0], [103.0, 3.0], [102.0, 3.0], [102.0, 2.0]]],
	 *       [[[100.0, 0.0], [101.0, 0.0], [101.0, 1.0], [100.0, 1.0], [100.0, 0.0]]]
	 *   ]
	 * }
	 * @endcode
	 */
	struct Multi_Polygon
	{
		/// Array of polygon geometries.
		std::vector<Polygon> polygons;
	};
}

#endif // GEOJSON_GEOMETRY_MULTI_POLYGON_H