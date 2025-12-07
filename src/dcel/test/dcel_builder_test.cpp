#include <gtest/gtest.h>


#include "dcel_builder_test.h"

#include "dcel/builder.h"
#include "io/feature_parser.h"

#include "hole_exemple.h"
#include "multi_polygon_exemple.h"
#include "reverse_exemple.h"
#include "simple_exemple.h"

#include <utils/zip.h>

class Auto_Builder : public O::DCEL::Builder, public O::GeoJSON::IO::Feature_Parser<Auto_Builder> {
public:
	using O::DCEL::Builder::On_Full_Feature;
	using O::DCEL::Builder::On_Root;
};

TYPED_TEST_SUITE_P(DCEL_Builder_Test);

TYPED_TEST_P(DCEL_Builder_Test, Vertex)
{

	Auto_Builder auto_builder;
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
		for (auto&& [out_edge_index, expected_out_edge_index] : O::Zip(vertex.outgoing_edges, expected_out_edge))
		{
			EXPECT_EQ(out_edge_index, expected_out_edge_index);
		}
	}
}

TYPED_TEST_P(DCEL_Builder_Test, Half_Edge)
{
    Auto_Builder auto_builder;
	rapidjson::StringStream ss(TypeParam::json.c_str());
	rapidjson::Reader reader;
	ASSERT_TRUE(reader.Parse(ss, auto_builder));
	auto opt_dcel = auto_builder.Get_Dcel();
	ASSERT_TRUE(opt_dcel.has_value());
	auto& dcel = opt_dcel.value();

	for (auto&& [half_edge, origin] : O::Zip(dcel.half_edges, TypeParam::expected_origins))
	{
		EXPECT_EQ(half_edge.origin, origin);
	}

	for (auto&& [half_edge, twin] : O::Zip(dcel.half_edges, TypeParam::expected_twins))
	{
		EXPECT_EQ(half_edge.twin, twin);
	}

	for (auto&& [half_edge, prev] : O::Zip(dcel.half_edges, TypeParam::expected_prevs))
	{
		EXPECT_EQ(half_edge.prev, prev);
	}

	for (auto&& [half_edge, next] : O::Zip(dcel.half_edges, TypeParam::expected_nexts))
	{
		EXPECT_EQ(half_edge.next, next);
	}

	for (auto&& [half_edge, face] : O::Zip(dcel.half_edges, TypeParam::expected_faces))
	{
		EXPECT_EQ(half_edge.face, face);
	}
}

TYPED_TEST_P(DCEL_Builder_Test, Face)
{
    Auto_Builder auto_builder;
	rapidjson::StringStream ss(TypeParam::json.c_str());
	rapidjson::Reader reader;
	ASSERT_TRUE(reader.Parse(ss, auto_builder));
	auto opt_dcel = auto_builder.Get_Dcel();
	ASSERT_TRUE(opt_dcel.has_value());
	auto& dcel = opt_dcel.value();

	for (auto&& [face, edge] : O::Zip(dcel.faces, TypeParam::expected_edges))
	{
		EXPECT_EQ(face.edge, edge);
	}

	for (auto&& [face, feature] : O::Zip(dcel.faces, TypeParam::expected_features))
	{
		EXPECT_EQ(face.associated_feature, feature);
	}

	for (auto&& [face, feature] : O::Zip(dcel.faces, TypeParam::expected_outer_faces))
	{
		EXPECT_EQ(face.outer_face, feature);
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