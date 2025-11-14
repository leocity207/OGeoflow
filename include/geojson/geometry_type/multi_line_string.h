#ifndef GEOJSON_GEOMETRY_MULTI_LINE_STRING_H
#define GEOJSON_GEOMETRY_MULTI_LINE_STRING_H

#include <vector>
#include "line_string.h"

namespace O::GeoJSON
{
	/**
	 * @brief Represents a GeoJSON MultiLineString geometry.
	 *
	 * A MultiLineString contains an array of LineString geometries.
	 *
	 * Example JSON:
	 * @code
	 * {
	 *   "type": "MultiLineString",
	 *   "coordinates": [
	 *       [[102.0, 0.0], [103.0, 1.0]],
	 *       [[104.0, 0.0], [105.0, 1.0]]
	 *   ]
	 * }
	 * @endcode
	 */
	struct Multi_Line_String
	{
		/// Array of line string geometries.
		std::vector<Line_String> line_strings;
	};
}

#endif // GEOJSON_GEOMETRY_MULTI_LINE_STRING_H