#ifndef DCEL_TO_GEOJSON_H
#define DCEL_TO_GEOJSON_H

#include "dcel.h"
#include "feature_info.h"
#include "include/geojson/geojson.h"
#include "include/geojson/geometry_type/polygon.h"
#include "include/geojson/geometry_type/multi_polygon.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/object/feature_collection.h"

namespace DCEL
{
	class Exporter
	{
	public:
		static GeoJSON::GeoJSON Convert(const DCEL& dcel, const Feature_Info& info);

	private:
	
		static std::vector<GeoJSON::Position> Extract_Ring(const DCEL& dcel, size_t start_edge);
		static GeoJSON::Polygon Build_Polygon_From_Face(const DCEL& dcel, size_t face_idx);
		static std::vector<GeoJSON::Polygon> Collect_Polygons_For_Feature(const DCEL& dcel, size_t featureIdx);
	};
}

#endif // DCEL_TO_GEOJSON_H
