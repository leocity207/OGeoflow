#ifndef GEOJSON_GEOMETRY_MULTI_LINE_STRING_H
#define GEOJSON_GEOMETRY_MULTI_LINE_STRING_H

#include <vector>

#include "line_string.h"

namespace GeoJSON
{

	/**
	 * @brief MultiLineString geometry - array of LineString geometries
	 */
	struct Multi_Line_String
	{
		std::vector<Line_String> line_strings;
	};
}

#endif //GEOJSON_GEOMETRY_MULTI_LINE_STRING_H