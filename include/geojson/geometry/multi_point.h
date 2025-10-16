#ifndef GEOJSON_GEOMETRY_MULTI_POINT_H
#define GEOJSON_GEOMETRY_MULTI_POINT_H

#include <vector>
#include <memory>

#include "include/geojson/geometry.h"
#include "include/geojson/position.h"

namespace GeoJSON
{

	/**
	 * @brief MultiPoint geometry - array of positions
	 */
	struct Multi_Point : public Geometry {
		std::vector<Position> positions;
		Multi_Point() { type = Geometry_Type::MULTI_POINT; }
	};
}

#endif //GEOJSON_GEOMETRY_MULTI_POINT_H