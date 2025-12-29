#ifndef DCEL_STORAGE_HPP
#define DCEL_STORAGE_HPP

#include "dcel/storage.h"

//Utils
#include <utils/zip.h>

template<class Vertex, class Half_Edge, class Face>
O::DCEL::Storage<Vertex, Half_Edge, Face>::Storage(const O::Configuration::DCEL& config) :
	config(config),
	vertices(),
	half_edges(),
	faces(),
	vertex_lookup(),
	edge_lookup(),
	feature_to_faces()
{
	vertices.reserve(config.max_vertices);
	half_edges.reserve(config.max_half_edges);
	faces.reserve(config.max_faces);
	vertex_lookup.reserve(config.max_vertices);
	edge_lookup.reserve(config.max_half_edges);
	feature_to_faces.reserve(config.max_faces);
}

template<class Vertex, class Half_Edge, class Face>
Vertex& O::DCEL::Storage<Vertex, Half_Edge, Face>::Get_Or_Create_Vertex(double x, double y)
{
	uint64_t key = Vertex::Hash(x, y);
	auto it = vertex_lookup.find(key);
	if (it != vertex_lookup.end()) return *it->second;
	vertices.emplace_back(x, y);
	vertex_lookup.emplace(key, &vertices.back());
	return vertices.back();
}

template<class Vertex, class Half_Edge, class Face>
Half_Edge& O::DCEL::Storage<Vertex, Half_Edge, Face>::Get_Or_Create_Half_Edge(Vertex& origin,Vertex& head)
{
	uint64_t key = Half_Edge::Hash(origin, head);
	auto it = edge_lookup.find(key);
	if (it != edge_lookup.end()) return *it->second;
	// create new halfedge for origin->head
	half_edges.emplace_back(origin, head);
	edge_lookup.emplace(key, &half_edges.back());

	return half_edges.back();
}

template<class Vertex, class Half_Edge, class Face>
void O::DCEL::Storage<Vertex, Half_Edge, Face>::Links_twins( Half_Edge& edge, Half_Edge& twin)
{
	edge.twin = &twin;
	twin.twin = &edge;
}

template<class Vertex, class Half_Edge, class Face>
void O::DCEL::Storage<Vertex, Half_Edge, Face>::Insert_Edge_Sorted(const Vertex& vertex,Half_Edge& edge)
{
	double vx = edge.head->x - vertex.x;
	double vy = edge.head->y - vertex.y;

	auto pos = edge.tail->outgoing_edges.begin();
	for (; pos != edge.tail->outgoing_edges.end(); ++pos)
	{
		const Half_Edge& old_edge = **pos;
		if (edge == old_edge)
			return; // we already inserted this half edge in vertex
		const Vertex& old_head = *old_edge.head;
		double ox = old_head.x - edge.tail->x;
		double oy = old_head.y - edge.tail->y;

		double cross = ox * vy - oy * vx;
		if (cross < 0) break; // new edge is clockwise after old edge
	}

	edge.tail->outgoing_edges.insert(pos, &edge);
}

template<class Vertex, class Half_Edge, class Face>
void O::DCEL::Storage<Vertex, Half_Edge, Face>::Update_Around_Vertex(const Vertex& vertex)
{
	if (vertex.outgoing_edges.size() < 2) return;

	for (auto&& [half_edge_curr,half_edge_next] : O::Zip_Adjacent_Circular(vertex.outgoing_edges))
	{
		half_edge_next->twin->next = half_edge_curr;
		half_edge_curr->prev = half_edge_next->twin;
	}
}

#endif //DCEL_STORAGE_HPP
