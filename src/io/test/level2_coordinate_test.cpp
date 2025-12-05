#include "level2_coordinate_test.h"

#include <ranges>
#include "io/parser.h"
#include "parser_helper_test.h"

TEST_F(Level2_Coordinate_Test, Line_String_Without_Altitude) {
	std::string json = R"({
		"type": "LineString",
		"coordinates": [
			[100.0, 9.02],
			[101.0, 8.0],
			[102.0, 7.0],
			[103.0, 8.0]
		]
	})";

	auto geometry = Parse_To_Geometry(json);
	ASSERT_TRUE(geometry.Is_Line_String());
	auto& line = geometry.Get_Line_String();

	std::vector<ExpectedPos> expected = {
		{100.0, 9.02, std::nullopt},
		{101.0, 8.0,  std::nullopt},
		{102.0, 7.0,  std::nullopt},
		{103.0, 8.0,  std::nullopt},
	};
	AssertPositionsEquals(line.positions, expected);
}

TEST_F(Level2_Coordinate_Test, Line_String_With_Mixed_Altitude) {
	std::string json = R"({
		"type": "LineString",
		"coordinates": [
			[100.0, 9.02, 1000],
			[101.0, 8.0],
			[102.0, 7.0, -1000],
			[103.0, 8.0, -0.0]
		]
	})";

	auto geometry = Parse_To_Geometry(json);
	ASSERT_TRUE(geometry.Is_Line_String());
	auto& line = geometry.Get_Line_String();

	std::vector<ExpectedPos> expected = {
		{100.0, 9.02,  1000.0},
		{101.0, 8.0,   std::nullopt},
		{102.0, 7.0,  -1000.0},
		{103.0, 8.0,     -0.0}
	};
	AssertPositionsEquals(line.positions, expected);
}

TEST_F(Level2_Coordinate_Test, Multi_Point_Without_Altitude) {
	std::string json = R"({
		"type": "MultiPoint",
		"coordinates": [
			[100.0, 9.02],
			[101.0, 8.0],
			[102.0, 7.0],
			[103.0, 8.0]
		]
	})";

	auto geometry = Parse_To_Geometry(json);
	ASSERT_TRUE(geometry.Is_Multi_Point());
	auto& mp = geometry.Get_Multi_Point();

	std::vector<ExpectedPos> expected = {
		{100.0, 9.02, std::nullopt},
		{101.0, 8.0,  std::nullopt},
		{102.0, 7.0,  std::nullopt},
		{103.0, 8.0,  std::nullopt},
	};
	ASSERT_EQ(mp.points.size(), 4);
	AssertPositionsEquals(mp.points, expected);
}

TEST_F(Level2_Coordinate_Test, Multi_Point_With_Mixed_Altitude) {
	std::string json = R"({
		"type": "MultiPoint",
		"coordinates": [
			[100.0, 9.02, 1000],
			[101.0, 8.0],
			[102.0, 7.0, -1000],
			[103.0, 8.0, -0.0]
		]
	})";

	auto geometry = Parse_To_Geometry(json);
	ASSERT_TRUE(geometry.Is_Multi_Point());
	auto& mp = geometry.Get_Multi_Point();

	std::vector<ExpectedPos> expected = {
		{100.0, 9.02,   1000.0},
		{101.0, 8.0,    std::nullopt},
		{102.0, 7.0,   -1000.0},
		{103.0, 8.0,     -0.0}
	};
	ASSERT_EQ(mp.points.size(), 4);
	AssertPositionsEquals(mp.points, expected);
}

TEST_F(Level2_Coordinate_Test, With_Too_Much_Coordinate_In_Point) {
	std::string json = R"({
		"type": "LineString",
		"coordinates": [
			[100.0, 9.02, 1000, 5000],
			[101.0, 8.0],
			[102.0, 7.0, -1000]
		]
	})";

	Expect_Parse_Error(json, O::GeoJSON::IO::Error::COORDINATE_OVERSIZED);
}

TEST_F(Level2_Coordinate_Test, With_Not_Enough_Point) {
	std::string json = R"({
		"type": "LineString",
		"coordinates": [
			[100.0, 9.02, 1000]
		]
	})";

	Expect_Parse_Error(json, O::GeoJSON::IO::Error::NEED_AT_LEAST_TWO_POSITION_FOR_LINESTRING);
}

TEST_F(Level2_Coordinate_Test, With_Not_Enough_Coordinate_In_Point) {
	std::string json = R"({
		"type": "LineString",
		"coordinates": [
			[100.0, 9.02, 1000],
			[101.0, 8.0],
			[102.0, 7.0, -1000],
			[103.0]
		]
	})";

	Expect_Parse_Error(json, O::GeoJSON::IO::Error::COORDINATE_UNDERSIZED);
}

TEST_F(Level2_Coordinate_Test, With_Mismatch_Level) {
	// third element is a bare number instead of a position array
	std::string json = R"({
		"type": "LineString",
		"coordinates": [
			[100.0, 9.02, 1000],
			[101.0, 8.0],
			105.5
		]
	})";

	Expect_Parse_Error(json, O::GeoJSON::IO::Error::INCONSCISTENT_COORDINATE_LEVEL);
}