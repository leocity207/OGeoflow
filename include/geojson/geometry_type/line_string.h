#ifndef GEOJSON_GEOMETRY_LINE_STRING_H
#define GEOJSON_GEOMETRY_LINE_STRING_H

#include <vector>

#include "include/geojson/position.h"

namespace O::GeoJSON
{
	/**
	 * @brief LineString geometry - array of positions forming a line
	 */
	struct Line_String
	{
		std::vector<Position> positions;
	};
}

#endif //GEOJSON_GEOMETRY_LINE_STRING_H