#ifndef GEOJSON_GEOMETRY_COLLECTION_H
#define GEOJSON_GEOMETRY_COLLECTION_H

#include <vector>
#include <memory>

#include "include/geojson/geometry.h"
#include "include/geojson/position.h"

namespace GeoJSON
{

	/**
	 * @brief GeometryCollection - collection of Geometry objects
	 */
	struct Geometry_Collection : public Geometry {
		std::vector<std::unique_ptr<Geometry>> geometries;
		Geometry_Collection() { type = Geometry_Type::GEOMETRY_COLLECTION; }
	};
}

#endif //GEOJSON_GEOMETRY_COLLECTION_H