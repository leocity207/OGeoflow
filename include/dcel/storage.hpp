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
bool O::DCEL::Storage<Vertex, Half_Edge, Face>::Does_Vertex_Exist(double x, double y)
{
	uint64_t key = Vertex::Hash(x, y);
	auto it = vertex_lookup.find(key);
	return it != vertex_lookup.end();
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
bool O::DCEL::Storage<Vertex, Half_Edge, Face>::Does_Half_Edge_Exist(Vertex& origin_vertex, Vertex& head_vertex)
{
	uint64_t key = Half_Edge::Hash(origin_vertex, head_vertex);
	auto it = edge_lookup.find(key);
	return it != edge_lookup.end();
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

template<class Vertex, class Half_Edge, class Face>
bool O::DCEL::Storage<Vertex, Half_Edge, Face>::Move(Vertex& vertex, double new_x, double new_y)
{
	if(!Does_Vertex_Exist(new_x, new_y))
		return vertex.Move(new_x,new_y);
	else
	{
		// Check there is a direct link between the two vertex
		auto& other_vertex = Get_Or_Create_Vertex(new_x,new_y);
		if(!Does_Half_Edge_Exist(vertex, other_vertex))
			return false; // Half edge should exist otherwise the move means we are crossing a different vertex.
		auto linking_half_edge = Get_Or_Create_Half_Edge(vertex, other_vertex);
		if(!Merge(other_vertex, vertex, linking_half_edge)) [[unlikely]] return false;
		if(!Remove(linking_half_edge)) [[unlikely]] return false;
		if(!Remove(vertex)) [[unlikely]] return false;
		return true;
	}
}

template<class Vertex, class Half_Edge, class Face>
bool O::DCEL::Storage<Vertex, Half_Edge, Face>::Remove(Half_Edge& half_edge)
{
	if(!half_edge.twin || half_edge.prev || half_edge.next || half_edge.twin->prev || half_edge.twin->next) [[unlikely]] return false;
	if (!half_edges.back()->next || half_edges.back()->prev) return false;

	auto& half_edge_twin = *half_edge.twin;
	
	half_edge.prev->next = half_edge.next;
	half_edge.next->prev = half_edge.next;
	half_edge_twin.prev->next = half_edge_twin.next;
	half_edge_twin.prev->next = half_edge_twin.next;

	half_edges.back()->next->prev = &half_edge_twin;
	half_edges.back()->prev->next = &half_edge_twin;
	std::swap(half_edges.back(), half_edge_twin);
	half_edges.pop_back();

	half_edges.back()->next->prev = &half_edge;
	half_edges.back()->prev->next = &half_edge;
	std::swap(half_edges.back(), half_edge);
	half_edges.pop_back();

	half_edge.twin = &half_edge_twin;
	Links_twins(half_edge,half_edge_twin);
	return true;
}

template<class Vertex, class Half_Edge, class Face>
bool O::DCEL::Storage<Vertex, Half_Edge, Face>::Remove(Vertex& vertex)
{
	for (auto edge : vertices.back().outgoing_edges)
	{
		if(!edge || !edge->twin) [[unlikely]] return false;
		edge->tail = &vertex;
		edge->twin->head = &vertex;
	}
	std::swap(vertex, vertices.back());
	vertices.pop_back();
	return true;
}

template<class Vertex, class Half_Edge, class Face>
bool O::DCEL::Storage<Vertex, Half_Edge, Face>::Merge(Vertex& v_keep, Vertex& v_discard, Half_Edge& e_discard)
{
	if(!e_discard.prev || !e_discard.next || !e_discard.twin) [[unlikely]] return false;

	// Relink outgoing edge of v_discard to v_keep
	auto it = std::ranges::find(v_keep.outgoing_edges, &e_discard);
	if(it == v_keep.outgoing_edges.end()) [[unlikely]] return false;
	v_keep.outgoing_edges.erase(it);

	auto it = std::ranges::find(v_discard.outgoing_edges, &e_discard);
	if(it == v_discard.outgoing_edges.end()) [[unlikely]] return false;
	v_discard.outgoing_edges.erase(it);

	for(auto remaining_edge : v_discard.outgoing_edges)
	{
		if(remaining_edge == &v_discard) continue;
		if(!remaining_edge || !remaining_edge->twin) [[unlikely]] return false;
		Insert_Edge_Sorted(v_keep, *remaining_edge);
		// While we are here change head tail of kept edge to v_keep
		remaining_edge->tail = &v_keep;
		remaining_edge->twin->head = &v_keep;
	}

	// Relink next and prev of all edge connected to 
	e_discard.prev->next = e_discard.next;
	e_discard.next->prev = e_discard.prev;
	e_discard.twin->prev->next = e_discard.twin->next;
	e_discard.twin->next->prev = e_discard.twin->prev;
	return true;
}

#endif //DCEL_STORAGE_HPP
