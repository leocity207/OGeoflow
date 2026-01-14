#ifndef SRC_DCEL_TEST_MERGE_EXEMPLE_COMMON_H
#define SRC_DCEL_TEST_MERGE_EXEMPLE_COMMON_H

// STL
#include <ranges>

// DCEL
#include "dcel/storage.h"
#include "dcel/vertex.h"
#include "dcel/half_edge.h"
#include "dcel/face.h"

namespace Merge_Exemple
{
	struct Vertex
	{
		double x;
		double y;
		std::vector<size_t> outgoing_edges;
	};

	struct Half_Edge
	{
		size_t tail;
		size_t head;
		size_t twin;
		size_t prev;
		size_t next;
		size_t face;
	};

	struct Move
	{
		double new_x;
		double new_y;
		size_t vertex_id;
	};

	const auto dcel_config = O::Configuration::DCEL{
		1000,
		1000,
		1000,
		1e-4,
		O::Configuration::DCEL::Merge_Strategy::AT_FIRST
	};

	struct Half_Edge_Impl : public O::DCEL::Half_Edge<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>> {
		using O::DCEL::Half_Edge<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>>::Half_Edge;
	};

	using Classifier_Storage = O::DCEL::Storage<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>>;
	Classifier_Storage From_Vertex(const std::vector<Vertex>& v, const std::vector<Half_Edge>& e)
	{
		Classifier_Storage storage(dcel_config);
		// initialize vertex and edges
		for (size_t _ : std::views::iota(0ul, v.size()))
		{
			(void)_;
			storage.vertices.emplace_back(0.0, 0.0);
		}
		for (size_t _ : std::views::iota(0ul, e.size()))
		{
			(void)_;
			storage.half_edges.emplace_back(storage.vertices[0], storage.vertices[0]);
		}

		// initialize vertex
		for (size_t i : std::ranges::iota_view(0ul, v.size()))
		{
			storage.vertices[i].x = v[i].x;
			storage.vertices[i].y = v[i].y;
			for (size_t j : std::ranges::iota_view(0ul, v[i].outgoing_edges.size()))
			{
				storage.vertices[i].outgoing_edges.push_back(&storage.half_edges[v[i].outgoing_edges[j]]);
			}
		}

		//initialize edges
		for (size_t i : std::ranges::iota_view(0ul, e.size()))
		{
			storage.half_edges[i].head = &storage.vertices[e[i].head];
			storage.half_edges[i].tail = &storage.vertices[e[i].tail];
			storage.half_edges[i].twin = &storage.half_edges[e[i].twin];
			storage.half_edges[i].next = &storage.half_edges[e[i].next];
			storage.half_edges[i].prev = &storage.half_edges[e[i].prev];
			storage.half_edges[i].face = (O::DCEL::Face<Half_Edge_Impl>*)e[i].face;
		}
		return storage;
	}
}

#endif //SRC_DCEL_TEST_MERGE_EXEMPLE_COMMON_H