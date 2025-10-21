#include "level4_coordinate_test.h"

#include <ranges>
#include "include/io/parser.h"
#include "parser_helper_test.h"

TEST_F(Level4_Coordinate_Test, Multi_Polygon_Single_Polygon_No_Altitude) {
	std::string json = R"({
		"type": "MultiPolygon",
		"coordinates": [
			[
				[
					[100.0, 9.02],
					[101.0, 8.0],
					[102.0, 7.0],
					[100.0, 9.02]
				]
			]
		]
	})";

	auto geometry = Parse_To_Geometry(json);
	ASSERT_TRUE(geometry.Is_Multi_Polygon());
	auto& mp = geometry.Get_Multi_Polygon();
	ASSERT_EQ(mp.polygons.size(), 1u);

	std::vector<ExpectedPos> expected = {
		{100.0, 9.02, std::nullopt},
		{101.0, 8.0,  std::nullopt},
		{102.0, 7.0,  std::nullopt},
		{100.0, 9.02, std::nullopt}
	};
	// premier polygone, anneau 0
	ASSERT_EQ(mp.polygons[0].rings.size(), 1u);
	AssertPositionsEquals(mp.polygons[0].rings[0], expected);
}

TEST_F(Level4_Coordinate_Test, Multi_Polygon_Multiple_Simple_Polygons) {
	std::string json = R"({
		"type": "MultiPolygon",
		"coordinates": [
			[
				[
					[100.0, 9.02],
					[101.0, 8.0],
					[102.0, 7.0],
					[100.0, 9.02]
				]
			],
			[
				[
					[200.0, 19.02],
					[201.0, 18.0],
					[202.0, 17.0],
					[200.0, 19.02]
				]
			],
			[
				[
					[0.0, 0.0],
					[0.5, 0.0],
					[0.5, 0.5],
					[0.0, 0.0]
				]
			]
		]
	})";

	auto geometry = Parse_To_Geometry(json);
	ASSERT_TRUE(geometry.Is_Multi_Polygon());
	auto& mp = geometry.Get_Multi_Polygon();
	ASSERT_EQ(mp.polygons.size(), 3u);

	std::vector<ExpectedPos> p0 = {
		{100.0, 9.02, std::nullopt},
		{101.0, 8.0,  std::nullopt},
		{102.0, 7.0,  std::nullopt},
		{100.0, 9.02, std::nullopt}
	};
	std::vector<ExpectedPos> p1 = {
		{200.0, 19.02, std::nullopt},
		{201.0, 18.0,  std::nullopt},
		{202.0, 17.0,  std::nullopt},
		{200.0, 19.02, std::nullopt}
	};
	std::vector<ExpectedPos> p2 = {
		{0.0, 0.0,   std::nullopt},
		{0.5, 0.0,   std::nullopt},
		{0.5, 0.5,   std::nullopt},
		{0.0, 0.0,   std::nullopt}
	};

	AssertPositionsEquals(mp.polygons[0].rings[0], p0);
	AssertPositionsEquals(mp.polygons[1].rings[0], p1);
	AssertPositionsEquals(mp.polygons[2].rings[0], p2);
}

TEST_F(Level4_Coordinate_Test, Multi_Polygon_Complex_Polygons) {
	std::string json = R"({
		"type": "MultiPolygon",
		"coordinates": [
			[
				[
					[100.0, 9.02],
					[101.0, 8.0],
					[102.0, 7.0],
					[100.0, 9.02]
				],
				[
					[110.0, 19.02],
					[111.0, 18.0],
					[112.0, 17.0],
					[110.0, 19.02]
				]
			],
			[ 
				[
					[200.0, 29.02],
					[201.0, 28.0],
					[202.0, 27.0],
					[200.0, 29.02]
				],
				[
					[210.0, 39.02],
					[211.0, 38.0],
					[212.0, 37.0],
					[210.0, 39.02]
				],
				[
					[220.0, 49.02],
					[221.0, 48.0],
					[222.0, 47.0],
					[220.0, 49.02]
				]
			],
			[ 
				[
					[0.0, 0.0],
					[1.0, 0.0],
					[1.0, 1.0],
					[0.0, 0.0]
				]
			]
		]
	})";

	auto geometry = Parse_To_Geometry(json);
	ASSERT_TRUE(geometry.Is_Multi_Polygon());
	auto& mp = geometry.Get_Multi_Polygon();

	ASSERT_EQ(mp.polygons.size(), 3u);
	ASSERT_EQ(mp.polygons[0].rings.size(), 2u);
	ASSERT_EQ(mp.polygons[1].rings.size(), 3u);
	ASSERT_EQ(mp.polygons[2].rings.size(), 1u);

	// vérification rapide de quelques anneaux (pas besoin d'inspecter tout si on a confiance dans helpers)
	std::vector<ExpectedPos> ring00 = {
		{100.0, 9.02, std::nullopt},
		{101.0, 8.0,  std::nullopt},
		{102.0, 7.0,  std::nullopt},
		{100.0, 9.02, std::nullopt}
	};
	std::vector<ExpectedPos> ring10 = {
		{200.0, 29.02, std::nullopt},
		{201.0, 28.0,  std::nullopt},
		{202.0, 27.0,  std::nullopt},
		{200.0, 29.02, std::nullopt}
	};
	std::vector<ExpectedPos> ring21 = {
		{0.0, 0.0, std::nullopt},
		{1.0, 0.0, std::nullopt},
		{1.0, 1.0, std::nullopt},
		{0.0, 0.0, std::nullopt}
	};

	AssertPositionsEquals(mp.polygons[0].rings[0], ring00);
	AssertPositionsEquals(mp.polygons[1].rings[0], ring10);
	AssertPositionsEquals(mp.polygons[2].rings[0], ring21);
}

TEST_F(Level4_Coordinate_Test, Multi_Polygon_Mixed_Altitude) {
	std::string json = R"({
		"type": "MultiPolygon",
		"coordinates": [
			[
				[
					[100.0, 9.02, 1000],
					[101.0, 8.0],
					[102.0, 7.0, -500],
					[100.0, 9.02, 1000]
				]
			]
		]
	})";

	auto geometry = Parse_To_Geometry(json);
	ASSERT_TRUE(geometry.Is_Multi_Polygon());
	auto& mp = geometry.Get_Multi_Polygon();
	ASSERT_EQ(mp.polygons.size(), 1u);
	ASSERT_EQ(mp.polygons[0].rings.size(), 1u);

	std::vector<ExpectedPos> expected = {
		{100.0, 9.02, 1000.0},
		{101.0, 8.0,  std::nullopt},
		{102.0, 7.0, -500.0},
		{100.0, 9.02, 1000.0}
	};
	AssertPositionsEquals(mp.polygons[0].rings[0], expected);
}

TEST_F(Level4_Coordinate_Test, Multi_Polygon_Ring_Too_Small) {
	std::string json = R"({
		"type": "MultiPolygon",
		"coordinates": [
			[
				[
					[100.0, 9.02],
					[101.0, 8.0],
					[102.0, 7.0]
				]
			]
		]
	})";

	Expect_Parse_Error(json, IO::Error::Type::NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON);
}

TEST_F(Level4_Coordinate_Test, Multi_Polygon_Ring_Not_Closed) {
	std::string json = R"({
		"type": "MultiPolygon",
		"coordinates": [
			[
				[
					[100.0, 9.02],
					[101.0, 8.0],
					[102.0, 7.0],
					[103.0, 8.0]
				]
			]
		]
	})";

	Expect_Parse_Error(json, IO::Error::Type::POLYGON_NEED_TO_BE_CLOSED);
}

TEST_F(Level4_Coordinate_Test, Multi_Polygon_Mismatch_Level) {
	std::string json = R"({
		"type": "MultiPolygon",
		"coordinates": [
			[
				[
					[100.0, 9.02],
					[101.0, 8.0],
					[102.0, 7.0],
					[100.0, 9.02]
				]
			],
			[ 
				[200.0, 19.02],
				[201.0, 18.0],
				[202.0, 17.0],
				[200.0, 19.02]
			]
		]
	})";

	Expect_Parse_Error(json, IO::Error::Type::INCONSCISTENT_COORDINATE_LEVEL);
}
