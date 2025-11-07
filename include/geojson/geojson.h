#ifndef GEOJSON_GEOJSON_H
#define GEOJSON_GEOJSON_H


#include <variant>

#include "object/feature.h"
#include "object/feature_collection.h"
#include "object/geometry.h"
#include "bbox.h"

namespace GeoJSON
{
	/**
	 * @brief Main GeoJSON container
	 */
	struct GeoJSON
	{
		std::variant<Feature, Feature_Collection, Geometry> object;

		/// @brief Is Function
		bool Is_Feature()            const noexcept { return std::holds_alternative<Feature>(object); }
		bool Is_Feature_Collection() const noexcept { return std::holds_alternative<Feature_Collection>(object); }
		bool Is_Geometry()           const noexcept { return std::holds_alternative<Geometry>(object); }

		/// @brief Const Getter functions
		const Feature&            Get_Feature()             const noexcept { return std::get<Feature>(object); }
		const Feature_Collection& Get_Feature_Collection()  const noexcept { return std::get<Feature_Collection>(object); }
		const Geometry&           Get_Geometry()            const noexcept { return std::get<Geometry>(object); }

		/// @brief Getter functions
		Feature&            Get_Feature()             noexcept { return std::get<Feature>(object); }
		Feature_Collection& Get_Feature_Collection()  noexcept { return std::get<Feature_Collection>(object); }
		Geometry&           Get_Geometry()            noexcept { return std::get<Geometry>(object); }
	};

	enum class Key
	{
		TYPE,
		COORDINATES,
		FEATURES_COLLECTION,
		PROPERTIES,
		GEOMETRY_COLLECTION,
		GEOMETRY,
		ID,
		BBOX,
		FOREIGN
	};

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

	inline Key String_To_Key(std::string_view str)
	{
		if (str == "type")             return Key::TYPE;
		else if (str == "coordinates") return Key::COORDINATES;
		else if (str == "features")    return Key::FEATURES_COLLECTION;
		else if(str == "properties")   return Key::PROPERTIES;
		else if(str == "geometry")     return Key::GEOMETRY;
		else if (str == "geometries")  return Key::GEOMETRY_COLLECTION;
		else if(str == "id")           return Key::ID;
		else if(str == "bbox")         return Key::BBOX;
		else                           return Key::FOREIGN;
	}
}

#endif //GEOJSON_GEOJSON_H