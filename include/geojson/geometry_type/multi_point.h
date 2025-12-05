#ifndef GEOJSON_GEOMETRY_MULTI_POINT_H
#define GEOJSON_GEOMETRY_MULTI_POINT_H

#include <vector>
#include "geojson/position.h"

namespace O::GeoJSON
{
	/**
	 * @brief Represents a GeoJSON MultiPoint geometry.
	 *        A MultiPoint contains an array of individual Positions.
	 *
	 * Example JSON:
	 * @code
	 * {
	 *   "type": "MultiPoint",
	 *   "coordinates": [
	 *       [102.0, 0.0],
	 *       [103.0, 1.0],
	 *       [104.0, 0.0],
	 *       [105.0, 1.0]
	 *   ]
	 * }
	 * @endcode
	 */
	struct Multi_Point
	{
		/// Collection of point positions.
		std::vector<Position> points;
	};
}

#endif // GEOJSON_GEOMETRY_MULTI_POINT_H