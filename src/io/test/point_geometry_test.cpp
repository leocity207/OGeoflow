#include "point_geometry_test.h"

#include "include/geojson/geometry/point.h"

TEST_P(Point_Geometry_Test, Parse_Various_Points) {
	auto [desc, lon, lat, alt, has_alt] = GetParam();
	
	std::string json = std::format(R"({{
		"type": "Feature",
		"geometry": {{
			"type": "Point",
			"coordinates": [{}, {}{}]
		}},
		"properties": {{}}
	}})", lon, lat, has_alt ? std::format(", {}", alt) : "");

	auto result = IO::Parse_Geojson_String(json);
	ASSERT_EQ(result.features.size(), 1);
	ASSERT_NE(result.features[0].geometry, nullptr);
	
	auto* point = dynamic_cast<GeoJSON::Point*>(result.features[0].geometry.get());
	ASSERT_NE(point, nullptr);
	
	EXPECT_NEAR(point->position.longitude, lon, 1e-10);
	EXPECT_NEAR(point->position.latitude, lat, 1e-10);
	
	if (has_alt) {
		EXPECT_TRUE(point->position.altitude.has_value());
		EXPECT_NEAR(point->position.altitude.value(), alt, 1e-10);
	}
}

INSTANTIATE_TEST_SUITE_P(
	Point_Coordinates,
	Point_Geometry_Test,
	::testing::Values(
		std::make_tuple("Simple 2D", 100.0, 0.0, 0.0, false),
		std::make_tuple("Negative coords", -120.5, -45.5, 0.0, false),
		std::make_tuple("With altitude", 50.0, 25.0, 1000.0, true),
		std::make_tuple("Edge coordinates", 180.0, 90.0, -500.0, true),
		std::make_tuple("Zero coordinates", 0.0, 0.0, 0.0, false)
	)
);