#ifndef GEOJSON_GEOMETRY_H
#define GEOJSON_GEOMETRY_H

#include <vector>
#include <memory>

#include "position.h"



namespace GeoJSON
{

	/**
	 * @brief Geometry types as defined in RFC 7946
	 */
	enum class Geometry_Type {
		POINT,
		MULTI_POINT,
		LINE_STRING,
		MULTI_LINE_STRING,
		POLYGON,
		MULTI_POLYGON,
		GEOMETRY_COLLECTION,
		UNKNOWN
	};

	/**
	 * @brief Base Geometry structure
	 */
	struct Geometry {
		Geometry_Type type;
		virtual ~Geometry() = default;
	};

	struct Polygon;
	struct Point;
	struct Multi_Polygon;
	struct Multi_Point;
	struct Multi_Line_String;
	struct Line_String;
	struct Geometry_Collection;
}

#endif //GEOJSON_GEOMETRY_H