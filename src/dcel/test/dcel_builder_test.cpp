#include <gtest/gtest.h>

// DCEL
#include "dcel_builder_test.h"
#include "dcel/builder.h"

// IO
#include "io/feature_parser.h"

// EXEMPLE
#include "hole_exemple.h"
#include "multi_polygon_exemple.h"
#include "reverse_exemple.h"
#include "simple_exemple.h"

// UTILS
#include <utils/zip.h>

// CONFIGURATION
#include "configuration/dcel.h"

static O::Configuration::DCEL config{
	1000,
	1000,
	1000,
	1e-9,
	O::Configuration::DCEL::Merge_Strategy::AT_FIRST
};

class Auto_Builder : public O::DCEL::Builder::From_GeoJSON, public O::GeoJSON::IO::Feature_Parser<Auto_Builder> {
public:
	using O::DCEL::Builder::From_GeoJSON::On_Full_Feature;
	using O::DCEL::Builder::From_GeoJSON::On_Root;
	Auto_Builder(const O::Configuration::DCEL& conf) :
		O::DCEL::Builder::From_GeoJSON(conf),
		O::GeoJSON::IO::Feature_Parser<Auto_Builder>()
	{

	}
};

TYPED_TEST_SUITE_P(DCEL_Builder_Test);

TYPED_TEST_P(DCEL_Builder_Test, Vertex)
{

	Auto_Builder auto_builder(config);
	rapidjson::StringStream ss(TypeParam::json.c_str());
	rapidjson::Reader reader;
	ASSERT_TRUE(reader.Parse(ss, auto_builder));
	auto opt_dcel = auto_builder.Get_Dcel();
	ASSERT_TRUE(opt_dcel.has_value());
	auto& dcel = opt_dcel.value();
	for (auto&& [vertex, expected_coord] : O::Zip(dcel.vertices, TypeParam::expected_coords))
	{
		SCOPED_TRACE(std::format("vertex: {} {} but expected: {} {}", vertex.x, vertex.y, expected_coord.first, expected_coord.second));
		EXPECT_EQ(vertex.x, expected_coord.first);
		EXPECT_EQ(vertex.y, expected_coord.second);
	}

	for (auto&& [vertex, expected_out_edge] : O::Zip(dcel.vertices, TypeParam::expected_out_edges))
	{
		ASSERT_EQ(vertex.outgoing_edges.size(), expected_out_edge.size());
		for (auto&& [out_edge, expected_out_edge_index] : O::Zip(vertex.outgoing_edges, expected_out_edge))
		{
			EXPECT_EQ(out_edge, &dcel.half_edges[expected_out_edge_index]);
		}
	}
}

TYPED_TEST_P(DCEL_Builder_Test, Half_Edge)
{
    Auto_Builder auto_builder(config);
	rapidjson::StringStream ss(TypeParam::json.c_str());
	rapidjson::Reader reader;
	ASSERT_TRUE(reader.Parse(ss, auto_builder));
	auto opt_dcel = auto_builder.Get_Dcel();
	ASSERT_TRUE(opt_dcel.has_value());
	auto& dcel = opt_dcel.value();

	for (auto&& [half_edge, tail_index] : O::Zip(dcel.half_edges, TypeParam::expected_tails))
	{
		EXPECT_EQ(half_edge.tail, &dcel.vertices[tail_index]);
	}

	for (auto&& [half_edge, twin_index] : O::Zip(dcel.half_edges, TypeParam::expected_twins))
	{
		EXPECT_EQ(half_edge.twin, &dcel.half_edges[twin_index]);
	}

	for (auto&& [half_edge, prev_index] : O::Zip(dcel.half_edges, TypeParam::expected_prevs))
	{
		EXPECT_EQ(half_edge.prev, &dcel.half_edges[prev_index]);
	}

	for (auto&& [half_edge, next_index] : O::Zip(dcel.half_edges, TypeParam::expected_nexts))
	{
		EXPECT_EQ(half_edge.next, &dcel.half_edges[next_index]);
	}

	for (auto&& [half_edge, face_index] : O::Zip(dcel.half_edges, TypeParam::expected_faces))
	{
		EXPECT_EQ(half_edge.face, &dcel.faces[face_index]);
	}
}

TYPED_TEST_P(DCEL_Builder_Test, Face)
{
    Auto_Builder auto_builder(config);
	rapidjson::StringStream ss(TypeParam::json.c_str());
	rapidjson::Reader reader;
	ASSERT_TRUE(reader.Parse(ss, auto_builder));
	auto opt_dcel = auto_builder.Get_Dcel();
	ASSERT_TRUE(opt_dcel.has_value());
	auto& dcel = opt_dcel.value();

	for (auto&& [face, edge_index] : O::Zip(dcel.faces, TypeParam::expected_edges))
	{
		EXPECT_EQ(face.edge, &dcel.half_edges[edge_index]);
	}
}

REGISTER_TYPED_TEST_SUITE_P(
    DCEL_Builder_Test,
    Vertex,
    Half_Edge,
    Face
);

// Instantiate for all ts
using All_Test_Sets = ::testing::Types<
	Simple_Exemple,
	Reverse_Exemple,
	Hole_Exemple,
	Multi_Polygon_Exemple
>;

INSTANTIATE_TYPED_TEST_SUITE_P(DCEL, DCEL_Builder_Test, All_Test_Sets);