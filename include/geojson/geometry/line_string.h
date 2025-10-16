#ifndef GEOJSON_GEOMETRY_LINE_STRING_H
#define GEOJSON_GEOMETRY_LINE_STRING_H

#include <vector>
#include <memory>

#include "include/geojson/geometry.h"
#include "include/geojson/position.h"

namespace GeoJSON
{
	/**
	 * @brief LineString geometry - array of positions forming a line
	 */
	struct Line_String : public Geometry {
		std::vector<Position> positions;
		Line_String() { type = Geometry_Type::LINE_STRING; }
	};
}

#endif //GEOJSON_GEOMETRY_LINE_STRING_H