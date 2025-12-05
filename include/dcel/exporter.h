#ifndef DCEL_TO_GEOJSON_H
#define DCEL_TO_GEOJSON_H

#include "storage.h"
#include "feature_info.h"
#include "geojson/root.h"
#include "geojson/geometry_type/polygon.h"
#include "geojson/geometry_type/multi_polygon.h"
#include "geojson/object/feature.h"
#include "geojson/object/feature_collection.h"

namespace O::DCEL
{
	/**
	 * @brief Exporter can be used to reconstruct back a GeoJSON object from A DCEL Storage and a FeatureInfo
	 */
	class Exporter
	{
	public:
		/**
		 * @brief Convert back the DCEL Storage and Feature_Info to a GeoJSON Root object
		 * @param dcel the DCEL::Storage for reconstruction
		 * @param info the Feature_INFO
		 * @return O::GeoJSON::Root 
		 */
		static O::GeoJSON::Root Convert(const Storage& dcel, const Feature_Info& info);

	private:
	
		/**
		 * @brief Extract Ring from the Storage with a given starting Edge
		 * @param dcel the DCEL::Storage for reconstruction
		 * @param start_edge a starting edge for a ring
		 * @return std::vector<O::GeoJSON::Position> a newly created ring
		 */
		static std::vector<O::GeoJSON::Position> Extract_Ring(const Storage& dcel, size_t start_edge);

		/**
		 * @brief Given a DCEL::Face Object Recreate the Polygon associated to it
		 * @param dcel the DCEL::Storage for reconstruction
		 * @param face_id the face_id to be found inside the Storage list
		 * @return O::GeoJSON::Polygon the reconstructed Polygon
		 */
		static O::GeoJSON::Polygon Build_Polygon_From_Face(const Storage& dcel, size_t face_id);

		/**
		 * @brief Given a feature id reconstruct all the polygon associated to it
		 * @param dcel the DCEL::Storage for reconstruction
		 * @param feature_id feature index that can be found inside the ``Feature_Info``
		 * @return std::vector<O::GeoJSON::Polygon> 
		 */
		static std::vector<O::GeoJSON::Polygon> Collect_Polygons_For_Feature(const Storage& dcel, size_t feature_id);
	};
}

#endif // DCEL_TO_GEOJSON_H
