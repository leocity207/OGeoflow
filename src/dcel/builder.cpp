#include "include/dcel/builder.h"
#include <cassert>


bool DCEL::Builder::On_Full_Feature(::GeoJSON::Feature&& feature)
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

bool  DCEL::Builder::On_Root(std::optional<::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id)
{
	m_feature_info.root_bbox = std::move(bbox);
	m_feature_info.root_id = std::move(id);
	m_feature_info.has_root = true;
	return true;
}

std::optional<DCEL::DCEL> DCEL::Builder::Get_Dcel()
{
	if(m_valid_dcel)
	{
		m_valid_dcel = false;
		return static_cast<DCEL>(std::move(m_dcel));
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

bool DCEL::Builder::On_Polygon(const ::GeoJSON::Polygon& poly, size_t feature_id)
{
    if (poly.rings.empty()) return true;
    Build_Face_From_Rings(poly.rings, feature_id);
    return true;
}

bool DCEL::Builder::On_MultiPolygon(const ::GeoJSON::Multi_Polygon& mp, size_t feature_id)
{
    for (auto const& poly : mp.polygons)
        On_Polygon(poly, feature_id);
    return true;
}

std::vector<size_t> DCEL::Builder::Create_Vertex(const std::vector<::GeoJSON::Position>& ring)
{
	std::vector<size_t> ring_vertex_indices; 
	ring_vertex_indices.reserve(ring.size());

	// create/get vertices
	for (size_t i = 0; i < ring.size() - 1; ++i)
	{
		double lon = ring[i].longitude;
		double lat = ring[i].latitude;
		size_t vid = m_dcel.Get_Or_Create_Vertex(lon, lat);
		ring_vertex_indices.push_back(vid);
	}
	// test the closed loop, the last vertex always exist and should be the same as the first one
	assert(m_dcel.Get_Or_Create_Vertex(ring[ring.size() - 1].longitude, ring[ring.size() - 1].latitude) == m_dcel.Get_Or_Create_Vertex(ring[0].longitude, ring[0].latitude));
	return ring_vertex_indices;
}

std::vector<size_t> DCEL::Builder::Create_Forward_Half_Edge(const std::vector<size_t>& ring_vertex_indices)
{
	std::vector<size_t> ring_edge_indices; ring_edge_indices.reserve(ring_vertex_indices.size());
	for (size_t i = 0; i < ring_vertex_indices.size(); ++i)
	{
		size_t origin = ring_vertex_indices[i];
		size_t head = ring_vertex_indices[(i + 1) % ring_vertex_indices.size()];
		// ensure halfedge origin->head exists
		size_t e_idxf = m_dcel.Get_Or_Create_Half_Edge_Index(origin, head);
		size_t e_idxb = m_dcel.Get_Or_Create_Half_Edge_Index(head, origin);
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
	for (size_t i = 0; i < ring_edge_indices.size(); ++i)
	{
		size_t e = ring_edge_indices[i];
		size_t e_next = ring_edge_indices[(i + 1) % ring_edge_indices.size()];
		size_t e_prev = ring_edge_indices[(i + ring_edge_indices.size() - 1) % ring_edge_indices.size()];
		m_dcel.halfedges[e].next = e_next;
		m_dcel.halfedges[e].prev = e_prev;
		// (face will be assigned below)
	}
}

size_t DCEL::Builder::Link_Face(const std::vector<size_t>& ring_edge_indices,size_t feature_id, bool isHole, size_t outer_Face_Idx )
{
		m_dcel.faces.emplace_back(ring_edge_indices[0], feature_id);
		for (size_t e : ring_edge_indices) 
			m_dcel.halfedges[e].face = m_dcel.faces.size() - 1;
		m_dcel.feature_to_faces[feature_id].push_back(m_dcel.faces.size() - 1);
		if (isHole && outer_Face_Idx != NO_IDX) 
		{
			m_dcel.faces.back().outer_face = outer_Face_Idx;
			m_dcel.faces[outer_Face_Idx].inner_edges.push_back(ring_edge_indices[0]);
		}
		return m_dcel.faces.size() - 1;
}

void DCEL::Builder::Build_Face_From_Rings(const std::vector<std::vector<::GeoJSON::Position>>& rings, size_t feature_id)
{
	assert(!rings.empty());

	// estimate and reserve
	size_t addVerticesEstimate = 0;
	size_t addEdgeSegments = 0;
	for (auto const& r : rings) { addVerticesEstimate += r.size(); addEdgeSegments += r.size(); }
	m_dcel.vertices.reserve(m_dcel.vertices.size() + addVerticesEstimate);
	m_dcel.halfedges.reserve(m_dcel.halfedges.size() + addEdgeSegments * 2);
	m_dcel.faces.reserve(m_dcel.faces.size() + rings.size());

	// For each ring, create or reuse vertices and halfedges (origin->head)
	// We'll record forward edge indices for the ring in the same order for face assignment.
	std::vector<size_t> created_vertices; created_vertices.reserve(addVerticesEstimate);
	size_t outer_face_idx = NO_IDX;

	for (size_t ringIndex = 0; ringIndex < rings.size(); ++ringIndex)
	{
		const auto& ring = rings[ringIndex];
		assert(ring.size() >= 4);


		std::vector<size_t> ring_vertex_indices = Create_Vertex(ring);
		std::vector<size_t> ring_edge_indices = Create_Forward_Half_Edge(ring_vertex_indices);
		Link_Next_Prev(ring_edge_indices);

		if (!(ringIndex > 0)) 
			outer_face_idx = Link_Face(ring_edge_indices, feature_id, false, NO_IDX);
		else
			Link_Face(ring_edge_indices, feature_id, true, outer_face_idx);

		// For each vertex touched by this ring, update around-vertex ordering and link twin->next / prev
		// We'll update only the vertices in this ring to limit per-feature work.
		for (size_t vid : ring_vertex_indices)
			m_dcel.Update_Around_Vertex(vid);
	} // end for each ring
};