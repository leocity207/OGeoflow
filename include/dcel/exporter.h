#ifndef DCEL_TO_GEOJSON_H
#define DCEL_TO_GEOJSON_H

// DCEL
#include "storage.h"
#include "feature_info.h"
#include "geojson/root.h"

// GEOMETRY
#include "geojson/geometry_type/polygon.h"
#include "geojson/geometry_type/multi_polygon.h"
#include "geojson/object/feature.h"
#include "geojson/object/feature_collection.h"

namespace O::DCEL::Exporter
{
	/**
	 * @brief Exporter can be used to reconstruct back a GeoJSON object from A DCEL Storage and a FeatureInfo
	 */
	template<class Vertex, class Half_Edge, class Face>
	class To_GeoJSON
	{
	public:
		/**
		 * @brief Convert back the DCEL Storage and Feature_Info to a GeoJSON Root object
		 * @param dcel the DCEL::Storage for reconstruction
		 * @param info the Feature_INFO
		 * @return O::GeoJSON::Root 
		 */
		static O::GeoJSON::Root Convert(const Feature_Info<Face>& info);

	private:
	
		/**
		 * @brief Extract Ring from the Storage with a given starting Edge
		 * @param face the face where to extract all vertices
		 * @return std::vector<O::GeoJSON::Position> a newly created ring
		 */
		static std::vector<O::GeoJSON::Position> Extract_Ring(const Face& face_id);


		/**
		 * @brief Collect all the face index for a given feature
		 * @param dcel the DCEL::Storage for reconstruction
		 * @param feature_id feature index that can be found inside the ``Feature_Info``
		 * @return std::vector<Unowned_Ptr<Face>> containing index of the feature
		 */
		static std::vector<Unowned_Ptr<const Face>> Collect_Face_From_Feature_ID(const Storage<Vertex, Half_Edge, Face>& dcel, size_t feature_id);


		/**
		 * @brief create a dictionary from face/outer_face to polygone
		 * @param dcel the DCEL::Storage for reconstruction
		 * @param face the list of face to sort in polygone
		 * @return O::GeoJSON::Polygon the reconstructed Polygon
		 */
		static GeoJSON::Polygon Create_Polygones( const std::vector<Unowned_Ptr<Face>>& faces);
	};
}

#include "exporter.hpp"

#endif // DCEL_TO_GEOJSON_H
