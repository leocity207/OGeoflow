#include <gtest/gtest.h>

#include "dcel/builder.h"

#include "io/feature_parser.h"

#include <utils/zip.h>

class Auto_Builder : public O::DCEL::Builder, public O::GeoJSON::IO::Feature_Parser<Auto_Builder> {
public:
	// If Builder�s On_Full_Feature/On_Root are enough, no need to override them
	using O::DCEL::Builder::On_Full_Feature;
	using O::DCEL::Builder::On_Root;
};

const std::string json = R"({
		"type": "FeatureCollection",
		"features": [
			{
				"type": "Feature",
				"geometry": { 
					"type": "Polygon",
					"coordinates": [[[0, 0], [1, 0], [1, 1], [0, 0]]] 
				},
				"properties": {}
			},
			{
				"type": "Feature",
				"geometry": { 
					"type": "Polygon",
					"coordinates": [[[1, 0], [1, 1], [2, 0], [1, 0]]] 
				},
				"properties": {}
			},
			{
				"type": "Feature",
				"geometry": { 
					"type": "Polygon",
					"coordinates": [[[0, 0], [1, 0], [2, 0], [3, -1], [2, -2], [0, -2], [0, 0]]] 
				},
				"properties": {}
			}
		]
})";

TEST(DCEL_Test, Vertex_Equality)
{
	Auto_Builder auto_builder;
	rapidjson::StringStream ss(json.c_str());
	rapidjson::Reader reader;
	ASSERT_TRUE(reader.Parse(ss, auto_builder));
	auto opt_dcel = auto_builder.Get_Dcel();
	ASSERT_TRUE(opt_dcel.has_value());
	auto& dcel = opt_dcel.value();
	auto expected_coords = std::to_array<std::pair<double, double>>({
		{0,0},
		{1,0},
		{1,1},
		{2,0},
		{3,-1},
		{2,-2},
		{0,-2}
	});

	ASSERT_EQ(dcel.vertices.size(), 7);
	for (auto&& [vertex, expected_coord] : O::Zip(dcel.vertices, expected_coords))
	{
		SCOPED_TRACE(std::format("vertex: {} {} but expected: {} {}", vertex.x, vertex.y, expected_coord.first, expected_coord.second));
		EXPECT_EQ(vertex.x, expected_coord.first);
		EXPECT_EQ(vertex.y, expected_coord.second);
	}

	const auto expected_out_edges = std::vector<std::vector<int>>{
		{16,0, 5},
		{8, 2, 1},
		{4, 3, 7},
		{6, 9, 15},
		{14,13},
		{12,11},
		{10,17}
	};

	for (auto&& [vertex, expected_out_edge] : O::Zip(dcel.vertices, expected_out_edges))
	{
		ASSERT_EQ(vertex.outgoing_edges.size(), expected_out_edge.size());
		for (auto&& [out_edge_index, expected_out_edge_index] : O::Zip(vertex.outgoing_edges, expected_out_edge))
		{
			EXPECT_EQ(out_edge_index, expected_out_edge_index);
		}
	}
}


TEST(DCEL_Test, Half_Edge_Equality)
{
	Auto_Builder auto_builder;
	rapidjson::StringStream ss(json.c_str());
	rapidjson::Reader reader;
	ASSERT_TRUE(reader.Parse(ss, auto_builder));
	auto opt_dcel = auto_builder.Get_Dcel();
	ASSERT_TRUE(opt_dcel.has_value());
	auto& dcel = opt_dcel.value();

	const auto origins = std::vector<int>{
		0,
		1,
		1,
		2,
		2,
		0,
		3,
		2,
		1,
		3,
		6,
		5,
		5,
		4,
		4,
		3,
		0,
		6
	};

	const auto twins = std::vector<int>{
		1,0,
		3,2,
		5,4,
		7,6,
		9,8,
		11,10,
		13,12,
		15,14,
		17,16
	};

	const auto prevs = std::vector<int>{
		4,
		9,
		0,
		6,
		2,
		17,
		8,
		5,
		3,
		14,
		16,
		13,
		10,
		15,
		12,
		7,
		1,
		11,
	};

	const auto nexts = std::vector<int>{
		2,
		16,
		4,
		8,
		0,
		7,
		3,
		15,
		6,
		1,
		12,
		17,
		14,
		11,
		9,
		13,
		10,
		5
	};

	const auto faces = std::vector<int>{
		0,
		2,
		0,
		1,
		0,
		3,
		1,
		3,
		1,
		2,
		2,
		3,
		2,
		3,
		2,
		3,
		2,
		3
	};

	for (auto&& [half_edge, origin] : O::Zip(dcel.half_edges, origins))
	{
		EXPECT_EQ(half_edge.origin, origin);
	}

	for (auto&& [half_edge, twin] : O::Zip(dcel.half_edges, twins))
	{
		EXPECT_EQ(half_edge.twin, twin);
	}

	for (auto&& [half_edge, prev] : O::Zip(dcel.half_edges, prevs))
	{
		EXPECT_EQ(half_edge.prev, prev);
	}

	for (auto&& [half_edge, next] : O::Zip(dcel.half_edges, nexts))
	{
		EXPECT_EQ(half_edge.next, next);
	}

	for (auto&& [half_edge, face] : O::Zip(dcel.half_edges, faces))
	{
		EXPECT_EQ(half_edge.face, face);
	}
}