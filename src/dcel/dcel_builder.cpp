#include "include/dcel/storage_builder_helper.h"

using namespace O;

size_t DCEL::Storage_Builder_Helper::Get_Or_Create_Vertex(double x, double y)
{
	uint64_t key = Vertex::Hash(x,y);
	auto it = vertex_lookup.find(key);
	if (it != vertex_lookup.end()) return it->second;
	size_t idx = vertices.size();
	vertices.emplace_back();
	vertices.back().x = x;
	vertices.back().y = y;
	vertex_lookup.emplace(key, idx);
	return idx;
}

uint64_t DCEL::Storage_Builder_Helper::EdgeKey(size_t originIdx, size_t headIdx) noexcept
{
	return ( (uint64_t)originIdx << 32 ) | (uint64_t)headIdx;
}


size_t DCEL::Storage_Builder_Helper::Get_Or_Create_Half_Edge_Index(size_t origin_id, size_t head_id)
{
	uint64_t key = EdgeKey(origin_id, head_id);
	auto it = edge_lookup.find(key);
	if (it != edge_lookup.end()) return it->second;

	// create new halfedge for origin->head
	size_t e_idx = halfedges.size();
	halfedges.emplace_back(origin_id);
	edge_lookup.emplace(key, halfedges.size() - 1);

	return e_idx;
}

void DCEL::Storage_Builder_Helper::Links_twins(size_t edge, size_t twin)
{
	auto& edge_ref = halfedges[edge];
	auto& twin_ref = halfedges[twin];

	edge_ref.twin = twin;
	twin_ref.twin = edge;
}


void DCEL::Storage_Builder_Helper::Insert_Edge_Sorted(size_t vertex_id, size_t edge_id)
{
	const Half_Edge& eNew = halfedges[edge_id];
	size_t head_id = halfedges[eNew.twin].origin;
	const Vertex& head = vertices[head_id];
	Vertex& tail = vertices[vertex_id];
	double vx = head.x - tail.x;
	double vy = head.y - tail.y;

	auto pos = tail.outgoing_edges.begin();
	for (; pos != tail.outgoing_edges.end(); ++pos)
	{
		size_t old_edge_id = *pos;
		size_t old_edge_head = halfedges[halfedges[old_edge_id].twin].origin;
		const Vertex& oldHead = vertices[old_edge_head];
		double ox = oldHead.x - tail.x;
		double oy = oldHead.y - tail.y;

		double cross = ox * vy - oy * vx;
		if (cross < 0) break; // new edge is clockwise after old edge
	}

	tail.outgoing_edges.insert(pos, edge_id);
}

void DCEL::Storage_Builder_Helper::Update_Around_Vertex(size_t vertexIdx)
{
	Vertex& v = vertices[vertexIdx];
	if (v.outgoing_edges.size() < 2) return;

	for (size_t i = 0; i < v.outgoing_edges.size(); ++i)
	{
		size_t e_i    = v.outgoing_edges[i];
		size_t e_next = v.outgoing_edges[(i + 1) % v.outgoing_edges.size()];

		size_t twin_i = halfedges[e_i].twin;
		if (twin_i == NO_IDX) continue;

		halfedges[twin_i].next = e_next;
		halfedges[e_next].prev = twin_i;
	}
}
