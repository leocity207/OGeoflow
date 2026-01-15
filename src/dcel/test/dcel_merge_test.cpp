#include <gtest/gtest.h>

#include <iostream>

// DCEL
#include "dcel/storage.h"

// EXEMPLE
#include "merge_exemple/x_exemple.h"
#include "merge_exemple/aligned_exemple.h"

template<typename T>
class Merge_Test : public ::testing::Test {};
TYPED_TEST_SUITE_P(Merge_Test);


TYPED_TEST_P(Merge_Test, Nominal)
{

	Merge_Exemple::Classifier_Storage dcel = Merge_Exemple::From_Vertex(TypeParam::vertices, TypeParam::edges);

	for (auto& half_edge :dcel.half_edges)
	{
		EXPECT_EQ(dcel.edge_lookup[Merge_Exemple::Half_Edge_Impl::Hash(*half_edge.tail, *half_edge.head)], &half_edge);
	}

	for (auto object : TypeParam::moves)
	{
		auto& vertex_to_move = dcel.vertices[object.vertex_id];
		ASSERT_TRUE(dcel.Move(vertex_to_move, object.new_x, object.new_y));
	}

	for (auto&& [vertex, expected_vertex] : O::Zip(dcel.vertices, TypeParam::expected_vertices))
	{
		//auto d_v = std::distance(dcel.vertices.begin(), std::ranges::find_if(dcel.vertices, [&](O::DCEL::Shemapify::Vertex& a) {return &a == &vertex; }));

		//EXPECT_NEAR(vertex.x, expected_vertex.x, 0.0001);
		//EXPECT_NEAR(vertex.y, expected_vertex.y, 0.0001);
		std::cout << vertex.x << "," << vertex.y << "{";
		for (auto&& [half_edge, expected_half_edge] : O::Zip(vertex.outgoing_edges, expected_vertex.outgoing_edges))
		{
			auto d = std::distance(dcel.half_edges.begin(), std::ranges::find_if(dcel.half_edges, [&](Merge_Exemple::Half_Edge_Impl& a) {return &a == half_edge; }));
			std::cout << d << ",";
			//EXPECT_EQ(d, expected_half_edge);
		}
		std::cout<< "}" << std::endl;

		//EXPECT_EQ(dcel.vertex_lookup[O::DCEL::Vertex<Merge_Exemple::Half_Edge_Impl>::Hash(vertex.x, vertex.y)], &vertex);
	}

	for (auto&& [half_edge, expected_half_edge] : O::Zip(dcel.half_edges, TypeParam::expected_edges))
	{
		//auto d_h = std::distance(dcel.half_edges.begin(), std::ranges::find_if(dcel.half_edges, [&](O::DCEL::Shemapify::Half_Edge& a) {return &a == &half_edge; }));

		auto d_tail = std::distance(dcel.vertices.begin(), std::ranges::find_if(dcel.vertices,     [&](O::DCEL::Vertex<Merge_Exemple::Half_Edge_Impl>& a) {return &a == half_edge.tail; }));
		auto d_head = std::distance(dcel.vertices.begin(), std::ranges::find_if(dcel.vertices,     [&](O::DCEL::Vertex<Merge_Exemple::Half_Edge_Impl>& a) {return &a == half_edge.head; }));
		auto d_twin = std::distance(dcel.half_edges.begin(), std::ranges::find_if(dcel.half_edges, [&](Merge_Exemple::Half_Edge_Impl& a) {return &a == half_edge.twin; }));
		auto d_next = std::distance(dcel.half_edges.begin(), std::ranges::find_if(dcel.half_edges, [&](Merge_Exemple::Half_Edge_Impl& a) {return &a == half_edge.prev; }));
		auto d_prev = std::distance(dcel.half_edges.begin(), std::ranges::find_if(dcel.half_edges, [&](Merge_Exemple::Half_Edge_Impl& a) {return &a == half_edge.next; }));
		auto d_face = (size_t)((O::DCEL::Face<Merge_Exemple::Half_Edge_Impl>*)half_edge.face);
		std::cout<< d_tail << "," << d_head << "," << d_twin << "," << d_next << "," << d_prev << "," << d_face << std::endl;

		//EXPECT_EQ(d_tail, expected_half_edge.tail);
		//EXPECT_EQ(d_head, expected_half_edge.head);
		//EXPECT_EQ(d_twin, expected_half_edge.twin);
		//EXPECT_EQ(d_next, expected_half_edge.prev);
		//EXPECT_EQ(d_prev, expected_half_edge.next);
		//EXPECT_EQ(d_face, expected_half_edge.face);
		//EXPECT_EQ(dcel.edge_lookup[Merge_Exemple::Half_Edge_Impl::Hash(*half_edge.tail, *half_edge.head)], &half_edge);
	}
}

REGISTER_TYPED_TEST_SUITE_P(
	Merge_Test,
	Nominal
);

// Instantiate for all ts
using All_Test_Sets = ::testing::Types<
	X_Exemple,
	Aligned_Exemple
>;

INSTANTIATE_TYPED_TEST_SUITE_P(DCEL, Merge_Test, All_Test_Sets);