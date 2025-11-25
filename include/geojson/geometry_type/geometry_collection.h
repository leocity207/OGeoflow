#ifndef GEOJSON_GEOMETRY_COLLECTION_H
#define GEOJSON_GEOMETRY_COLLECTION_H

#include <vector>
#include <memory>

namespace O::GeoJSON
{
	struct Geometry;

	/**
	 * @brief Represents a GeoJSON GeometryCollection.
	 *
	 * A GeometryCollection is a container of multiple geometry objects of any supported type (Point, LineString, Polygon, etc.).
	 *
	 * Example JSON:
	 * @code
	 * {
	 *   "type": "GeometryCollection",
	 *   "geometries": [
	 *       { "type": "Point", "coordinates": [100.0, 0.0] },
	 *       { "type": "LineString", "coordinates": [[101.0, 0.0], [102.0, 1.0]] }
	 *   ]
	 * }
	 * @endcode
	 */
	struct Geometry_Collection
	{
		/// Vector of geometry objects, stored as shared pointers.
		std::vector<std::shared_ptr<Geometry>> geometries;
	};
}

#endif // GEOJSON_GEOMETRY_COLLECTION_H