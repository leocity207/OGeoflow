#ifndef GEOJSON_GEOMETRY_POLYGONE_H
#define GEOJSON_GEOMETRY_POLYGONE_H

#include <vector>
#include "include/geojson/position.h"

namespace O::GeoJSON
{
	/**
	 * @brief Represents a GeoJSON Polygon geometry.
	 *
	 * A Polygon is an array of linear rings, where:
	 * - The first ring defines the outer boundary (exterior).
	 * - Subsequent rings define interior holes (if any).
	 *
	 * Example JSON:
	 * @code
	 * {
	 *   "type": "Polygon",
	 *   "coordinates": [
	 *       [[100.0, 0.0], [101.0, 0.0], [101.0, 1.0], [100.0, 1.0], [100.0, 0.0]],
	 *       [[100.2, 0.2], [100.8, 0.2], [100.8, 0.8], [100.2, 0.8], [100.2, 0.2]]
	 *   ]
	 * }
	 * @endcode
	 */
	struct Polygon
	{
		/// Array of linear rings; each ring is a closed list of positions.
		std::vector<std::vector<Position>> rings;
	};
}

#endif // GEOJSON_GEOMETRY_POLYGONE_H