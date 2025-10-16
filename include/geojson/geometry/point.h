#ifndef GEOJSON_GEOMETRY_POINT_H
#define GEOJSON_GEOMETRY_POINT_H

#include <vector>
#include <memory>

#include "include/geojson/geometry.h"
#include "include/geojson/position.h"

namespace GeoJSON
{
	/**
	 * @brief Point geometry - single position
	 */
	struct Point : public Geometry {
		Position position;
		Point() { type = Geometry_Type::POINT; }
		explicit Point(const Position& pos) : position(pos) { type = Geometry_Type::POINT; }
	};
}

#endif //GEOJSON_GEOMETRY_POINT_H