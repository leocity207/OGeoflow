#include "dcel/builder.h"

// STD
#include <cassert>
#include <algorithm>
#include <ranges>

// UTILS
#include <utils/zip.h>

using namespace O;

DCEL::Builder::From_GeoJSON::From_GeoJSON(const O::Configuration::DCEL& config) :
	m_dcel(config),
	m_valid_dcel(true),
	m_valid_feature_info(true),
	m_feature_info()
{

}

bool DCEL::Builder::From_GeoJSON::Parse(GeoJSON::Root&& geojson)
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

bool DCEL::Builder::From_GeoJSON::On_Full_Feature(GeoJSON::Feature&& feature)
{
	if (!feature.geometry) return true;
	m_feature_info.feature_properties.emplace_back(std::move(feature.properties));
	m_feature_info.bboxes.emplace_back(std::move(feature.bbox));
	m_feature_info.ids.emplace_back(std::move(feature.id));

	auto& geom = *feature.geometry;
	if (geom.Is_Polygon())
		On_Polygon(geom.Get_Polygon());
	else if (geom.Is_Multi_Polygon())
		On_MultiPolygon(geom.Get_Multi_Polygon());
	return true;
}

bool  DCEL::Builder::From_GeoJSON::On_Root(std::optional<GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id)
{
	m_feature_info.root_bbox = std::move(bbox);
	m_feature_info.root_id = std::move(id);
	m_feature_info.has_root = true;
	return true;
}

void DCEL::Builder::From_GeoJSON::Link_Outer_Bound_Face()
{
	for (Half_Edge& half_edge: m_dcel.half_edges)
	{
		if (half_edge.face)
			continue;
		m_dcel.faces.emplace_back( &half_edge );
		Half_Edge* e = &half_edge;
		do {
			e->face = &m_dcel.faces.back();
			e = e->next;
			assert(e);
		} while (*e != half_edge);
	}
}

std::optional<DCEL::Storage> DCEL::Builder::From_GeoJSON::Get_Dcel()
{

	// finish face for outer bound
	Link_Outer_Bound_Face();

	if(m_valid_dcel)
	{
		m_valid_dcel = false;
		return static_cast<Storage>(std::move(m_dcel));
	}
	return std::nullopt;
}

std::optional<DCEL::Feature_Info> DCEL::Builder::From_GeoJSON::Get_Feature_Info()
{
	if(m_valid_feature_info)
	{
		m_valid_feature_info = false;
		return std::move(m_feature_info);
	}
	return std::nullopt;
}

bool DCEL::Builder::From_GeoJSON::On_Polygon(const GeoJSON::Polygon& poly)
{
    if (poly.rings.empty()) return true;
    auto faces = Build_Face_From_Rings(poly.rings);
	std::vector<std::vector<O::Unowned_Ptr<Face>>> polygon({ std::move(faces) });
	m_feature_info.faces.emplace_back(std::move(polygon));
    return true;
}

bool DCEL::Builder::From_GeoJSON::On_MultiPolygon(const GeoJSON::Multi_Polygon& mp)
{
	m_feature_info.faces.push_back({});
	for (auto const& poly : mp.polygons)
	{
		if (poly.rings.empty()) return true;
		auto faces = Build_Face_From_Rings(poly.rings);
		m_feature_info.faces.back().emplace_back(std::move(faces));
	}
    return true;
}

std::vector<Unowned_Ptr<DCEL::Vertex>> DCEL::Builder::From_GeoJSON::Create_Vertex(const std::vector<GeoJSON::Position>& ring)
{
	std::vector<Unowned_Ptr<DCEL::Vertex>> ring_vertices;
	ring_vertices.reserve(ring.size());

	// create/get vertices
	double area = 0;
	for (auto&& [ring_i, ring_i_1] : O::Zip_Adjacent(ring))
	{
		double lon = ring_i.longitude;
		double lat = ring_i.latitude;
		Vertex& vid = m_dcel.Get_Or_Create_Vertex(lon, lat);
		ring_vertices.emplace_back(&vid);
		area = ring_i.longitude * ring_i_1.latitude - ring_i.latitude + ring_i_1.longitude;
	}

	if (area > 0) // reverse so ring circle are always counter clock wise
		std::ranges::reverse(ring_vertices);
	return ring_vertices;
}

std::vector<Unowned_Ptr<DCEL::Half_Edge>> DCEL::Builder::From_GeoJSON::Create_Forward_Half_Edge(const std::vector<Unowned_Ptr<Vertex>>& ring_vertex)
{
	std::vector<Unowned_Ptr<DCEL::Half_Edge>> ring_edges;
	ring_edges.reserve(ring_vertex.size());
	for (auto&& [origin, head] : O::Zip_Adjacent_Circular(ring_vertex))
	{
		// ensure halfedge origin->head exists
		Half_Edge& half_edge = m_dcel.Get_Or_Create_Half_Edge(*origin, *head);
		Half_Edge& half_edge_twin = m_dcel.Get_Or_Create_Half_Edge(*head, *origin);
		m_dcel.Links_twins(half_edge, half_edge_twin);
		m_dcel.Insert_Edge_Sorted(*origin, half_edge);
		m_dcel.Insert_Edge_Sorted(*head, half_edge_twin);
		ring_edges.emplace_back(&half_edge);
		ring_edges.emplace_back(&half_edge_twin);
	}
	return ring_edges;
}

void DCEL::Builder::From_GeoJSON::Link_Next_Prev(const std::vector<O::Unowned_Ptr<Half_Edge>>& ring_edges)
{
	for (size_t i : std::views::iota(0ul, ring_edges.size()))
	{
		Half_Edge& e = *ring_edges[i];
		Half_Edge& e_next = *ring_edges[(i + 1) % ring_edges.size()];
		Half_Edge& e_prev = *ring_edges[(i + ring_edges.size() - 1) % ring_edges.size()];
		e.next = &e_next;
		e.prev = &e_prev;
	}
}

DCEL::Face& DCEL::Builder::From_GeoJSON::Link_Face(std::vector<O::Unowned_Ptr<Half_Edge>>& ring_edge, O::Unowned_Ptr<Face> outer_face)
{
	Half_Edge& valid_start = (outer_face == nullptr) ? *ring_edge[0] : *ring_edge[1];
	O::Unowned_Ptr<Half_Edge> current(&valid_start);
	m_dcel.faces.emplace_back(&valid_start);
	do {
		current->face = &m_dcel.faces.back();
		current = current->next;
	}
	while(current != &valid_start);
	return m_dcel.faces.back();
}

std::vector<O::Unowned_Ptr<DCEL::Face>> DCEL::Builder::From_GeoJSON::Build_Face_From_Rings(const std::vector<std::vector<GeoJSON::Position>>& rings)
{
	assert(!rings.empty());

	// estimate and reserve

	// For each ring, create or reuse vertices and halfedges (origin->head)
	// We'll record forward edge for the ring in the same order for face assignment.
	O::Unowned_Ptr<Face> outer_face = nullptr;
	std::vector<O::Unowned_Ptr<Face>> created_faces;
	for (auto&& [ring, ring_index] : O::Zip_Index(rings))
	{
		assert(ring.size() >= 4);

		std::vector<Unowned_Ptr<Vertex>> ring_vertices = Create_Vertex(ring);
		std::vector<Unowned_Ptr<Half_Edge>> ring_edges = Create_Forward_Half_Edge(ring_vertices);
		Link_Next_Prev(ring_edges);
		for (Unowned_Ptr<Vertex> vid : ring_vertices)
			m_dcel.Update_Around_Vertex(*vid);
		
		if (ring_index > 0)
		{
			created_faces.emplace_back(&Link_Face(ring_edges, outer_face));
		}
		else
		{
			outer_face = &Link_Face(ring_edges, nullptr);
			created_faces.emplace_back(outer_face);
		}
	} // end for each ring
	return created_faces;
};