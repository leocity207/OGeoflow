#ifndef GEOJSON_GEOMETRY_H
#define GEOJSON_GEOMETRY_H

#include <variant>
#include <memory>
#include <string>

#include "geojson/geometry_type/line_string.h"
#include "geojson/geometry_type/multi_line_string.h"
#include "geojson/geometry_type/multi_point.h"
#include "geojson/geometry_type/multi_polygon.h"
#include "geojson/geometry_type/point.h"
#include "geojson/geometry_type/polygon.h"
#include "geojson/geometry_type/geometry_collection.h"
#include "geojson/bbox.h"


namespace O::GeoJSON
{

	/**
	 * @brief Represents the base GeoJSON Geometry object.
	 *
	 * A Geometry defines the spatial form of a GeoJSON entity. It encapsulates one of the seven supported geometry types (`Point`, `MultiPoint`, `LineString`, `MultiLineString`, `Polygon`, `MultiPolygon`, or `GeometryCollection`) inside a variant union.
	 *
	 * This structure acts as the unified interface for all geometry variants and provides utility methods to test and retrieve the active geometry type.
	 *
	 * @see RFC 7946 §3.1 — Geometry Objects
	 *
	 * Example JSON:
	 * @code
	 * {
	 *   "type": "Polygon",
	 *   "coordinates": [
	 *       [[100.0, 0.0], [101.0, 0.0], [101.0, 1.0], [100.0, 1.0], [100.0, 0.0]]
	 *   ]
	 * }
	 * @endcode
	 */
	struct Geometry
	{
		/**
		 * @brief Enumerates the supported GeoJSON geometry types.
		 */
		enum class Type
		{
			POINT,               ///< GeoJSON "Point"
			MULTI_POINT,         ///< GeoJSON "MultiPoint"
			LINE_STRING,         ///< GeoJSON "LineString"
			MULTI_LINE_STRING,   ///< GeoJSON "MultiLineString"
			POLYGON,             ///< GeoJSON "Polygon"
			MULTI_POLYGON,       ///< GeoJSON "MultiPolygon"
			GEOMETRY_COLLECTION, ///< GeoJSON "GeometryCollection"
			UNKNOWN              ///< Undefined or unsupported type
		};

		/// Holds one of the seven possible geometry types.
		std::variant<
			Point,
			Multi_Point,
			Line_String,
			Multi_Line_String,
			Polygon,
			Multi_Polygon,
			Geometry_Collection
		> value;

		/// Optional bounding box defining the spatial extent of this geometry.
		std::optional<Bbox> bbox;

		/**
		 * @name Type Checkers
		 * @{
		 * @brief Convenience methods to test which geometry type is currently held.
		 */
		bool Is_Point()               const noexcept { return std::holds_alternative<Point>(value); }
		bool Is_Multi_Point()         const noexcept { return std::holds_alternative<Multi_Point>(value); }
		bool Is_Line_String()         const noexcept { return std::holds_alternative<Line_String>(value); }
		bool Is_Multi_Line_String()   const noexcept { return std::holds_alternative<Multi_Line_String>(value); }
		bool Is_Polygon()             const noexcept { return std::holds_alternative<Polygon>(value); }
		bool Is_Multi_Polygon()       const noexcept { return std::holds_alternative<Multi_Polygon>(value); }
		bool Is_Geometry_Collection() const noexcept { return std::holds_alternative<Geometry_Collection>(value); }
		/** @} */

		/**
		 * @name Constant Accessors
		 * @{
		 * @brief Return a const reference to the geometry currently held.
		 */
		const Point&               Get_Point()               const noexcept { return std::get<Point>(value); }
		const Multi_Point&         Get_Multi_Point()         const noexcept { return std::get<Multi_Point>(value); }
		const Line_String&         Get_Line_String()         const noexcept { return std::get<Line_String>(value); }
		const Multi_Line_String&   Get_Multi_Line_String()   const noexcept { return std::get<Multi_Line_String>(value); }
		const Polygon&             Get_Polygon()             const noexcept { return std::get<Polygon>(value); }
		const Multi_Polygon&       Get_Multi_Polygon()       const noexcept { return std::get<Multi_Polygon>(value); }
		const Geometry_Collection& Get_Geometry_Collection() const noexcept { return std::get<Geometry_Collection>(value); }
		/** @} */

		/**
		 * @name Mutable Accessors
		 * @{
		 * @brief Return a modifiable reference to the stored geometry variant.
		 */
		Point&               Get_Point()               noexcept { return std::get<Point>(value); }
		Multi_Point&         Get_Multi_Point()         noexcept { return std::get<Multi_Point>(value); }
		Line_String&         Get_Line_String()         noexcept { return std::get<Line_String>(value); }
		Multi_Line_String&   Get_Multi_Line_String()   noexcept { return std::get<Multi_Line_String>(value); }
		Polygon&             Get_Polygon()             noexcept { return std::get<Polygon>(value); }
		Multi_Polygon&       Get_Multi_Polygon()       noexcept { return std::get<Multi_Polygon>(value); }
		Geometry_Collection& Get_Geometry_Collection() noexcept { return std::get<Geometry_Collection>(value); }
		/** @} */

		/**
		 * @brief Convert a geometry type name string into its enum representation.
		 * @param type_str The textual representation of the geometry type.
		 * @return The corresponding Geometry::Type enum value.
		 */
		inline Type String_To_Type(const std::string& type_str)
		{
			if (type_str == "Point")              return Type::POINT;
			else if (type_str == "MultiPoint")    return Type::MULTI_POINT;
			else if (type_str == "LineString")    return Type::LINE_STRING;
			else if (type_str == "MultiLineString") return Type::MULTI_LINE_STRING;
			else if (type_str == "Polygon")       return Type::POLYGON;
			else if (type_str == "MultiPolygon")  return Type::MULTI_POLYGON;
			else if (type_str == "GeometryCollection") return Type::GEOMETRY_COLLECTION;
			return Type::UNKNOWN;
		}
	};

} // namespace GeoJSON

#endif // GEOJSON_GEOMETRY_H
