#ifndef GEOJSON_GEOMETRY_MULTI_LINE_STRING_H
#define GEOJSON_GEOMETRY_MULTI_LINE_STRING_H

#include <vector>
#include <memory>

#include "include/geojson/geometry.h"
#include "include/geojson/position.h"

namespace GeoJSON
{

	/**
	 * @brief MultiLineString geometry - array of LineString geometries
	 */
	struct Multi_Line_String : public Geometry {
		std::vector<Line_String> lines;
		Multi_Line_String() { type = Geometry_Type::MULTI_LINE_STRING; }
	};
}

#endif //GEOJSON_GEOMETRY_MULTI_LINE_STRING_H