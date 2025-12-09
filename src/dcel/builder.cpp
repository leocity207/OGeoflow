#include "dcel/builder.h"

// STD
#include <cassert>
#include <algorithm>
#include <ranges>

// UTILS
#include <utils/zip.h>

using namespace O;

bool DCEL::Builder::Parse_GeoJSON(GeoJSON::Root&& geojson)
{
	return std::visit([&](auto&& val) {
		using T = std::decay_t<decltype(val)>;
		if constexpr (std::is_same_v<T, GeoJSON::Feature_Collection>)
		{
			for(auto&& feature : geojson.Get_Feature_Collection().features)
				On_Full_Feature(std::move(feature));
			On_Root(std::move(geojson.Get_Feature_Collection().bbox), std::move(geojson.Get_Feature_Collection().id));
			return true;
		}
		else if constexpr (std::is_same_v<T, GeoJSON::Feature>)
			return On_Full_Feature(std::move(geojson.Get_Feature()));
		else
			return false;
	}, geojson.object);

}

bool DCEL::Builder::On_Full_Feature(GeoJSON::Feature&& feature)
{
	if (!feature.geometry) return true;
	m_feature_info.feature_properties.emplace_back(std::move(feature.properties));
	m_feature_info.bboxes.emplace_back(std::move(feature.bbox));
	m_feature_info.ids.emplace_back(std::move(feature.id));

	auto& geom = *feature.geometry;
	if (geom.Is_Polygon())
		On_Polygon(geom.Get_Polygon(), m_feature_info.feature_properties.size() - 1);
	else if (geom.Is_Multi_Polygon())
		On_MultiPolygon(geom.Get_Multi_Polygon(), m_feature_info.feature_properties.size() - 1);
	return true;
}

bool  DCEL::Builder::On_Root(std::optional<GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id)
{
	m_feature_info.root_bbox = std::move(bbox);
	m_feature_info.root_id = std::move(id);
	m_feature_info.has_root = true;
	return true;
}

std::optional<DCEL::Storage> DCEL::Builder::Get_Dcel()
{

	// finish face for outer bound
	for (size_t i : std::views::iota(0ul, m_dcel.half_edges.size())) {
		if (m_dcel.half_edges[i].face == NO_IDX) {
			size_t unbounded_index = m_dcel.faces.size();
			m_dcel.faces.emplace_back(i, NO_IDX, NO_IDX);
			size_t start = i;
			size_t e = start;
			do {
				m_dcel.half_edges[e].face = unbounded_index;
				e = m_dcel.half_edges[e].next;
				if (e == NO_IDX) break;
			} while (e != start);
		}
	}

	if(m_valid_dcel)
	{
		m_valid_dcel = false;
		return static_cast<Storage>(std::move(m_dcel));
	}
	return std::nullopt;
}

std::optional<DCEL::Feature_Info> DCEL::Builder::Get_Feature_Info()
{
	if(m_valid_feature_info)
	{
		m_valid_feature_info = false;
		return std::move(m_feature_info);
	}
	return std::nullopt;
}

bool DCEL::Builder::On_Polygon(const GeoJSON::Polygon& poly, size_t feature_id)
{
    if (poly.rings.empty()) return true;
    Build_Face_From_Rings(poly.rings, feature_id);
    return true;
}

bool DCEL::Builder::On_MultiPolygon(const GeoJSON::Multi_Polygon& mp, size_t feature_id)
{
    for (auto const& poly : mp.polygons)
        On_Polygon(poly, feature_id);
    return true;
}

std::vector<size_t> DCEL::Builder::Create_Vertex(const std::vector<GeoJSON::Position>& ring)
{
	std::vector<size_t> ring_vertex_indices; 
	ring_vertex_indices.reserve(ring.size());

	// create/get vertices
	double area = 0;
	for (auto&& [ring_i, ring_i_1] : O::Zip_Adjacent(ring))
	{
		double lon = ring_i.longitude;
		double lat = ring_i.latitude;
		size_t vid = m_dcel.Get_Or_Create_Vertex(lon, lat);
		ring_vertex_indices.push_back(vid);
		area = ring_i.longitude * ring_i_1.latitude - ring_i.latitude + ring_i_1.longitude;
	}
	// test the closed loop, the last vertex always exist and should be the same as the first one
	assert(m_dcel.Get_Or_Create_Vertex(ring[ring.size() - 1].longitude, ring[ring.size() - 1].latitude) == m_dcel.Get_Or_Create_Vertex(ring[0].longitude, ring[0].latitude));

	if (area > 0) // reverse so ring circle are always counter clock wise
		std::ranges::reverse(ring_vertex_indices);
	return ring_vertex_indices;
}

std::vector<size_t> DCEL::Builder::Create_Forward_Half_Edge(const std::vector<size_t>& ring_vertex_indices)
{
	std::vector<size_t> ring_edge_indices; ring_edge_indices.reserve(ring_vertex_indices.size());
	for (auto&& [origin, head] : O::Zip_Adjacent_Circular(ring_vertex_indices))
	{
		// ensure halfedge origin->head exists
		size_t e_idxf = m_dcel.Get_Or_Create_Half_Edge(origin, head);
		size_t e_idxb = m_dcel.Get_Or_Create_Half_Edge(head, origin);
		m_dcel.Links_twins(e_idxf, e_idxb);
		m_dcel.Insert_Edge_Sorted(origin, e_idxf);
		m_dcel.Insert_Edge_Sorted(head, e_idxb);
		ring_edge_indices.push_back(e_idxf);
		ring_edge_indices.push_back(e_idxb);
	}
	return ring_edge_indices;
}

void DCEL::Builder::Link_Next_Prev(const std::vector<size_t>& ring_edge_indices)
{
	for (size_t i : std::views::iota(0ul, ring_edge_indices.size()))
	{
		size_t e = ring_edge_indices[i];
		size_t e_next = ring_edge_indices[(i + 1) % ring_edge_indices.size()];
		size_t e_prev = ring_edge_indices[(i + ring_edge_indices.size() - 1) % ring_edge_indices.size()];
		m_dcel.half_edges[e].next = e_next;
		m_dcel.half_edges[e].prev = e_prev;
		// (face will be assigned below)
	}
}

size_t DCEL::Builder::Link_Face(const std::vector<size_t>& ring_edge_indices, size_t feature_id, size_t outer_face_id)
{
	size_t valid_start_index = outer_face_id == NO_IDX ? ring_edge_indices[0] : ring_edge_indices[1];
	bool has_hit_another_face = false;
	size_t other_face_id;
	size_t current_index = valid_start_index;
	do {
		Half_Edge& current_edge = m_dcel.half_edges[current_index];
		if(current_edge.face != NO_IDX)
		{
			has_hit_another_face = true;
			other_face_id = current_edge.face;
		}
		current_edge.face = feature_id;
		current_index = current_edge.next;
	}
	while(current_index != valid_start_index);
	m_dcel.faces.emplace_back(valid_start_index, feature_id, outer_face_id);
	return feature_id;
}

void DCEL::Builder::Build_Face_From_Rings(const std::vector<std::vector<GeoJSON::Position>>& rings, size_t feature_id)
{
	assert(!rings.empty());

	// estimate and reserve
	size_t addVerticesEstimate = 0;
	size_t addEdgeSegments = 0;
	for (auto const& r : rings) { addVerticesEstimate += r.size(); addEdgeSegments += r.size(); }
	m_dcel.vertices.reserve(m_dcel.vertices.size() + addVerticesEstimate);
	m_dcel.half_edges.reserve(m_dcel.half_edges.size() + addEdgeSegments * 2);
	m_dcel.faces.reserve(m_dcel.faces.size() + rings.size());

	// For each ring, create or reuse vertices and halfedges (origin->head)
	// We'll record forward edge indices for the ring in the same order for face assignment.
	std::vector<size_t> created_vertices; created_vertices.reserve(addVerticesEstimate);
	size_t outer_face_id = NO_IDX;
	for (auto&& [ring, ringIndex] : O::Zip_Index(rings))
	{
		assert(ring.size() >= 4);

		std::vector<size_t> ring_vertex_indices = Create_Vertex(ring);
		std::vector<size_t> ring_edge_indices = Create_Forward_Half_Edge(ring_vertex_indices);
		Link_Next_Prev(ring_edge_indices);
		for (size_t vid : ring_vertex_indices)
			m_dcel.Update_Around_Vertex(vid);
		
		if(ringIndex > 0)
			Link_Face(ring_edge_indices, feature_id, outer_face_id);
		else
			outer_face_id = Link_Face(ring_edge_indices, feature_id, NO_IDX );
	} // end for each ring
};