#ifndef GEOJSON_GEOMETRY_POINT_H
#define GEOJSON_GEOMETRY_POINT_H

#include "include/geojson/position.h"

namespace O::GeoJSON
{
	/**
	 * @brief Represents a GeoJSON Point geometry.
	 *
	 * A Point holds a single Position, describing a single location in space.
	 *
	 * Example JSON:
	 * @code
	 * {
	 *   "type": "Point",
	 *   "coordinates": [102.0, 0.5]
	 * }
	 * @endcode
	 */
	struct Point
	{
		/// The geographic position of the point.
		Position position;
	};
}

#endif // GEOJSON_GEOMETRY_POINT_H
