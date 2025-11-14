#ifndef GEOJSON_GEOJSON_H
#define GEOJSON_GEOJSON_H

#include <variant>

#include "object/feature.h"
#include "object/feature_collection.h"
#include "object/geometry.h"
#include "bbox.h"

namespace O::GeoJSON
{
	/**
	 * @brief Root container representing any valid GeoJSON entity.
	 *
	 * The `GeoJSON` structure is the top-level abstraction that can hold
	 * a single geometry, a feature, or a feature collection — all defined
	 * by the official GeoJSON specification (RFC 7946).
	 *
	 * @details
	 * The internal `std::variant` ensures that at any given time, only one
	 * of these object types is active:
	 *
	 * - :cpp:struct:`GeoJSON::Geometry`
	 * - :cpp:struct:`GeoJSON::Feature`
	 * - :cpp:struct:`GeoJSON::Feature_Collection`
	 *
	 * This unified representation simplifies parsing, storage, and serialization
	 * by encapsulating all possible root-level GeoJSON objects in a single structure.
	 */
	struct Root
	{
		/// @brief The actual GeoJSON entity held (geometry, feature, or collection).
		std::variant<Feature, Feature_Collection, Geometry> object;

		/** @name Type Queries
		 *  @brief Helper methods to check which type of GeoJSON object is currently stored.
		 *  @{
		 */
		bool Is_Feature()            const noexcept { return std::holds_alternative<Feature>(object); }
		bool Is_Feature_Collection() const noexcept { return std::holds_alternative<Feature_Collection>(object); }
		bool Is_Geometry()           const noexcept { return std::holds_alternative<Geometry>(object); }
		/** @} */

		/** @name Const Accessors
		 *  @brief Provides read-only access to the stored GeoJSON object.
		 *  @{
		 */
		const Feature&            Get_Feature()             const noexcept { return std::get<Feature>(object); }
		const Feature_Collection& Get_Feature_Collection()  const noexcept { return std::get<Feature_Collection>(object); }
		const Geometry&           Get_Geometry()            const noexcept { return std::get<Geometry>(object); }
		/** @} */

		/** @name Mutable Accessors
		 *  @brief Provides mutable access to the stored GeoJSON object.
		 *  @{
		 */
		Feature&            Get_Feature()             noexcept { return std::get<Feature>(object); }
		Feature_Collection& Get_Feature_Collection()  noexcept { return std::get<Feature_Collection>(object); }
		Geometry&           Get_Geometry()            noexcept { return std::get<Geometry>(object); }
		/** @} */
	};

	/**
	 * @brief Enumerates the standard keys found in GeoJSON objects.
	 *
	 * Used primarily during parsing or serialization to identify known
	 * members of a GeoJSON document.
	 *
	 * @see String_To_Key
	 */
	enum class Key
	{
		TYPE,                 ///< The "type" member.
		COORDINATES,          ///< The "coordinates" member (geometry data).
		FEATURES_COLLECTION,  ///< The "features" array in a FeatureCollection.
		PROPERTIES,           ///< The "properties" member in a Feature.
		GEOMETRY_COLLECTION,  ///< The "geometries" array in a GeometryCollection.
		GEOMETRY,             ///< The "geometry" member in a Feature.
		ID,                   ///< The "id" member (optional identifier).
		BBOX,                 ///< The "bbox" member (bounding box).
		FOREIGN               ///< Any unrecognized/foreign key.
	};

	/**
	 * @brief Enumerates all possible GeoJSON object types.
	 *
	 * This enumeration maps directly to the `"type"` field found in GeoJSON
	 * documents. It allows strongly-typed handling of geometry and feature
	 * variants.
	 *
	 * @see String_To_Type
	 */
	enum class Type
	{
		POINT,
		MULTI_POINT,
		LINE_STRING,
		MULTI_LINE_STRING,
		POLYGON,
		MULTI_POLYGON,
		GEOMETRY_COLLECTION,
		FEATURE_COLLECTION,
		FEATURE,
		UNKNOWN
	};

	/**
	 * @brief Converts a string view to a corresponding GeoJSON type enumeration.
	 *
	 * @param type_str The value of the `"type"` field.
	 * @return The corresponding :cpp:enum:`GeoJSON::Type` or `Type::UNKNOWN` if unrecognized.
	 */
	inline Type String_To_Type(std::string_view type_str)
	{
		if (type_str == "Point")                   return Type::POINT;
		else if (type_str == "MultiPoint")         return Type::MULTI_POINT;
		else if (type_str == "LineString")         return Type::LINE_STRING;
		else if (type_str == "MultiLineString")    return Type::MULTI_LINE_STRING;
		else if (type_str == "Polygon")            return Type::POLYGON;
		else if (type_str == "MultiPolygon")       return Type::MULTI_POLYGON;
		else if (type_str == "GeometryCollection") return Type::GEOMETRY_COLLECTION;
		else if (type_str == "FeatureCollection")  return Type::FEATURE_COLLECTION;
		else if (type_str == "Feature")            return Type::FEATURE;
		else                                       return Type::UNKNOWN;
	}

	/**
	 * @brief Converts a string key into a strongly-typed GeoJSON key enumeration.
	 *
	 * @param str The name of a key found in the JSON document.
	 * @return A corresponding :cpp:enum:`GeoJSON::Key` value.
	 */
	inline Key String_To_Key(std::string_view str)
	{
		if (str == "type")             return Key::TYPE;
		else if (str == "coordinates") return Key::COORDINATES;
		else if (str == "features")    return Key::FEATURES_COLLECTION;
		else if (str == "properties")  return Key::PROPERTIES;
		else if (str == "geometry")    return Key::GEOMETRY;
		else if (str == "geometries")  return Key::GEOMETRY_COLLECTION;
		else if (str == "id")          return Key::ID;
		else if (str == "bbox")        return Key::BBOX;
		else                           return Key::FOREIGN;
	}
}

#endif // GEOJSON_GEOJSON_H