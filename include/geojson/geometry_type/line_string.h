#ifndef GEOJSON_GEOMETRY_LINE_STRING_H
#define GEOJSON_GEOMETRY_LINE_STRING_H

#include <vector>
#include "include/geojson/position.h"

namespace O::GeoJSON
{
	/**
	 * @brief Represents a GeoJSON LineString geometry.
	 *
	 * A LineString is an ordered array of two or more Positions forming a continuous line.
	 *
	 * Example JSON:
	 * @code
	 * {
	 *   "type": "LineString",
	 *   "coordinates": [
	 *       [102.0, 0.0],
	 *       [103.0, 1.0],
	 *       [104.0, 0.0],
	 *       [105.0, 1.0]
	 *   ]
	 * }
	 * @endcode
	 */
	struct Line_String
	{
		/// Ordered list of positions defining the line.
		std::vector<Position> positions;
	};
}

#endif // GEOJSON_GEOMETRY_LINE_STRING_H
