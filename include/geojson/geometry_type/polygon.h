#ifndef GEOJSON_GEOMETRY_POLYGONE_H
#define GEOJSON_GEOMETRY_POLYGONE_H

#include <vector>

#include "include/geojson/position.h"

namespace GeoJSON
{
	/**
	 * @brief Polygon geometry - array of linear rings
	 * @note First ring is exterior, subsequent rings are holes
	 */
	struct Polygon
	{
		std::vector<std::vector<Position>> rings;
	};
}

#endif //GEOJSON_GEOMETRY_POLYGONE_H