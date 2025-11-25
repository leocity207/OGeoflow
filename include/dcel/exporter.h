#ifndef DCEL_TO_GEOJSON_H
#define DCEL_TO_GEOJSON_H

#include "storage.h"
#include "feature_info.h"
#include "include/geojson/root.h"
#include "include/geojson/geometry_type/polygon.h"
#include "include/geojson/geometry_type/multi_polygon.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/object/feature_collection.h"

namespace O::DCEL
{
	class Exporter
	{
	public:
		static O::GeoJSON::Root Convert(const Storage& dcel, const Feature_Info& info);

	private:
	
		static std::vector<O::GeoJSON::Position> Extract_Ring(const Storage& dcel, size_t start_edge);
		static O::GeoJSON::Polygon Build_Polygon_From_Face(const Storage& dcel, size_t face_idx);
		static std::vector<O::GeoJSON::Polygon> Collect_Polygons_For_Feature(const Storage& dcel, size_t featureIdx);
	};
}

#endif // DCEL_TO_GEOJSON_H
