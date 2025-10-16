#ifndef GEOJSON_GEOMETRY_POLYGONE_H
#define GEOJSON_GEOMETRY_POLYGONE_H

#include <vector>
#include <memory>

#include "include/geojson/geometry.h"
#include "include/geojson/position.h"

namespace GeoJSON
{
	/**
	 * @brief Polygon geometry - array of linear rings
	 * @note First ring is exterior, subsequent rings are holes
	 */
	struct Polygon : public Geometry {
		std::vector<std::vector<Position>> rings;
		Polygon() { type = Geometry_Type::POLYGON; }
	};
}

#endif //GEOJSON_GEOMETRY_POLYGONE_H