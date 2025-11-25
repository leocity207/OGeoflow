#ifndef DCEL_BUILDER_H
#define DCEL_BUILDER_H


#include "storage_builder_helper.h"

#include "include/geojson/root.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/geometry_type/polygon.h"
#include "include/geojson/geometry_type/multi_polygon.h"
#include "feature_info.h"

namespace O::DCEL
{
	class Builder
	{
	public:
		bool Parse_GeoJSON(O::GeoJSON::Root&& geojson);
		bool On_Full_Feature(O::GeoJSON::Feature&& feature);
		bool On_Root(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);
		std::optional<Storage> Get_Dcel();
		std::optional<Feature_Info> Get_Feature_Info();

	private:
		void Build_Face_From_Rings(const std::vector<std::vector<O::GeoJSON::Position>>& rings,size_t featureIdx);
		bool On_Polygon(const O::GeoJSON::Polygon& poly, size_t feature_id);
		bool On_MultiPolygon(const O::GeoJSON::Multi_Polygon& mp, size_t feature_id);
		std::vector<size_t> Create_Vertex(const std::vector<O::GeoJSON::Position>& ring);
		std::vector<size_t> Create_Forward_Half_Edge(const std::vector<size_t>& ring_vertex_indices);
		void Link_Next_Prev(const std::vector<size_t>& ring_edge_indices);
		size_t Link_Face(const std::vector<size_t>& ring_edge_indices, size_t featureIdx, bool isHole, size_t outerFaceIdx);


	private:

		Storage_Builder_Helper m_dcel;
		bool m_valid_dcel = true;
		bool m_valid_feature_info = true;
		Feature_Info m_feature_info;
	};
}

#endif //DCEL_BUILDER_H