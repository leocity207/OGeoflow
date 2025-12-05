#include "dcel/storage_builder_helper.h"

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

size_t DCEL::Storage_Builder_Helper::Get_Or_Create_Half_Edge(size_t origin_id, size_t head_id)
{
	uint64_t key = Half_Edge::Hash(origin_id, head_id);
	auto it = edge_lookup.find(key);
	if (it != edge_lookup.end()) return it->second;

	// create new halfedge for origin->head
	size_t e_idx = half_edges.size();
	half_edges.emplace_back(origin_id);
	edge_lookup.emplace(key, half_edges.size() - 1);

	return e_idx;
}

void DCEL::Storage_Builder_Helper::Links_twins(size_t edge, size_t twin)
{
	auto& edge_ref = half_edges[edge];
	auto& twin_ref = half_edges[twin];

	edge_ref.twin = twin;
	twin_ref.twin = edge;
}


void DCEL::Storage_Builder_Helper::Insert_Edge_Sorted(size_t vertex_id, size_t edge_id)
{
	const Half_Edge& eNew = half_edges[edge_id];
	size_t head_id = half_edges[eNew.twin].origin;
	const Vertex& head = vertices[head_id];
	Vertex& tail = vertices[vertex_id];
	double vx = head.x - tail.x;
	double vy = head.y - tail.y;

	auto pos = tail.outgoing_edges.begin();
	for (; pos != tail.outgoing_edges.end(); ++pos)
	{
		size_t old_edge_id = *pos;
		size_t old_edge_head = half_edges[half_edges[old_edge_id].twin].origin;
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

		size_t twin_i = half_edges[e_i].twin;
		if (twin_i == NO_IDX) continue;

		half_edges[twin_i].next = e_next;
		half_edges[e_next].prev = twin_i;
	}
}
