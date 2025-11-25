#ifndef GEOJSON_GEOMETRY_COLLECTION_H
#define GEOJSON_GEOMETRY_COLLECTION_H

#include <vector>

namespace O::GeoJSON
{
	struct Geometry;
	/**
	 * @brief GeometryCollection - collection of Geometry objects
	 */
	struct Geometry_Collection
	{
		std::vector<std::shared_ptr<Geometry>> geometries;
	};
}

#endif //GEOJSON_GEOMETRY_COLLECTION_H