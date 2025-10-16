#ifndef GEOJSON_GEOMETRY_MULTI_POLYGONE_H
#define GEOJSON_GEOMETRY_MULTI_POLYGONE_H

#include <vector>
#include <memory>

#include "include/geojson/geometry.h"
#include "polygone.h"

namespace GeoJSON
{

	/**
	 * @brief MultiPolygon geometry - array of Polygon geometries
	 */
	struct Multi_Polygon : public Geometry {
		std::vector<Polygon> polygons;
		Multi_Polygon() { type = Geometry_Type::MULTI_POLYGON; }
	};
}

#endif //GEOJSON_GEOMETRY_MULTI_POLYGONE_H