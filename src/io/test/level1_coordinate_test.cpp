#include "level1_coordinate_test.h"

#include "include/io/parser.h"
#include "parser_helper_test.h"

TEST_F(Level1_Coordinate_Test, Point_Without_Altitude) {
	std::string json = R"({
		"type": "Point",
		"coordinates": [100.0, 9.02]
	})";

	auto geometry = Parse_To_Geometry(json);
	ASSERT_TRUE(geometry.Is_Point());
	auto& point = geometry.Get_Point();

	AssertPositionEquals(point.position, {100.0, 9.02, std::nullopt});
}

TEST_F(Level1_Coordinate_Test, Point_With_Altitude) {
	std::string json = R"({
		"type": "Point",
		"coordinates": [100.0, 9.02, -1200]
	})";

	auto geometry = Parse_To_Geometry(json);
	ASSERT_TRUE(geometry.Is_Point());
	auto& point = geometry.Get_Point();

	AssertPositionEquals(point.position, {100.0, 9.02, -1200.0});
}

TEST_F(Level1_Coordinate_Test, Point_Too_Many_Values) {
	std::string json = R"({
		"type": "Point",
		"coordinates": [100.0, 9.02, -1200, 0.0]
	})";
	Expect_Parse_Error(json, O::GeoJSON::IO::Error::COORDINATE_OVERSIZED);
}

TEST_F(Level1_Coordinate_Test, Point_Not_Enough_Values) {
	std::string json = R"({
		"type": "Point",
		"coordinates": [100]
	})";
	Expect_Parse_Error(json, O::GeoJSON::IO::Error::COORDINATE_UNDERSIZED);
}