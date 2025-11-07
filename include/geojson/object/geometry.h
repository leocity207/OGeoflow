#ifndef GEOJSON_GEOMETRY_H
#define GEOJSON_GEOMETRY_H

#include <variant>
#include <memory>
#include <string>

#include "include/geojson/geometry_type/line_string.h"
#include "include/geojson/geometry_type/multi_line_string.h"
#include "include/geojson/geometry_type/multi_point.h"
#include "include/geojson/geometry_type/multi_polygon.h"
#include "include/geojson/geometry_type/point.h"
#include "include/geojson/geometry_type/polygon.h"
#include "include/geojson/geometry_type/geometry_collection.h"
#include "include/geojson/bbox.h"




namespace GeoJSON
{

	/**
	 * @brief Base Geometry structure
	 */
	struct Geometry
	{
		enum class Type
		{
			POINT,
			MULTI_POINT,
			LINE_STRING,
			MULTI_LINE_STRING,
			POLYGON,
			MULTI_POLYGON,
			GEOMETRY_COLLECTION,
			UNKNOWN
		};

		std::variant<
			Point,
			Multi_Point,
			Line_String,
			Multi_Line_String,
			Polygon,
			Multi_Polygon,
			Geometry_Collection
		> value;
		std::optional<Bbox> bbox;

		bool Is_Point()               const noexcept { return std::holds_alternative<Point>(value); }
		bool Is_Multi_Point()         const noexcept { return std::holds_alternative<Multi_Point>(value); }
		bool Is_Line_String()         const noexcept { return std::holds_alternative<Line_String>(value); }
		bool Is_Multi_Line_String()   const noexcept { return std::holds_alternative<Multi_Line_String>(value); }
		bool Is_Polygon()             const noexcept { return std::holds_alternative<Polygon>(value); }
		bool Is_Multi_Polygon()       const noexcept { return std::holds_alternative<Multi_Polygon>(value); }
		bool Is_Geometry_Collection() const noexcept { return std::holds_alternative<Geometry_Collection>(value); }

		const Point&               Get_Point()               const noexcept { return std::get<Point>(value); }
		const Multi_Point&         Get_Multi_Point()         const noexcept { return std::get<Multi_Point>(value); }
		const Line_String&         Get_Line_String()         const noexcept { return std::get<Line_String>(value); }
		const Multi_Line_String&   Get_Multi_Line_String()   const noexcept { return std::get<Multi_Line_String>(value); }
		const Polygon&             Get_Polygon()             const noexcept { return std::get<Polygon>(value); }
		const Multi_Polygon&       Get_Multi_Polygon()       const noexcept { return std::get<Multi_Polygon>(value); }
		const Geometry_Collection& Get_Geometry_Collection() const noexcept { return std::get<Geometry_Collection>(value); }

		Point&               Get_Point()               noexcept { return std::get<Point>(value); }
		Multi_Point&         Get_Multi_Point()         noexcept { return std::get<Multi_Point>(value); }
		Line_String&         Get_Line_String()         noexcept { return std::get<Line_String>(value); }
		Multi_Line_String&   Get_Multi_Line_String()   noexcept { return std::get<Multi_Line_String>(value); }
		Polygon&             Get_Polygon()             noexcept { return std::get<Polygon>(value); }
		Multi_Polygon&       Get_Multi_Polygon()       noexcept { return std::get<Multi_Polygon>(value); }
		Geometry_Collection& Get_Geometry_Collection() noexcept { return std::get<Geometry_Collection>(value); }


		inline Type String_To_Type(const std::string& type_str)
		{
			if (type_str == "Point")              return Type::POINT;
			else if (type_str == "MultiPoint")         return Type::MULTI_POINT;
			else if (type_str == "LineString")         return Type::LINE_STRING;
			else if (type_str == "MultiLineString")    return Type::MULTI_LINE_STRING;
			else if (type_str == "Polygon")            return Type::POLYGON;
			else if (type_str == "MultiPolygon")       return Type::MULTI_POLYGON;
			else if (type_str == "GeometryCollection") return Type::GEOMETRY_COLLECTION;
			return Type::UNKNOWN;
		}
	};


}

#endif //GEOJSON_GEOMETRY_H