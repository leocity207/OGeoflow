#include "line_string_geometry_test.h"

#include "include/geojson/geometry/line_string.h"

TEST_P(Line_String_Geometry_Test, Parse_Line_Strings) {
	auto [desc, points] = GetParam();
	
	std::string coords = "";
	for (size_t i = 0; i < points.size(); ++i) {
		auto [lon, lat] = points[i];
		coords += std::format("[{}, {}]", lon, lat);
		if (i < points.size() - 1) coords += ", ";
	}
	
	std::string json = std::format(R"({{
		"type": "Feature",
		"geometry": {{
			"type": "LineString",
			"coordinates": [{}]
		}}
	}})", coords);

	auto result = IO::Parse_Geojson_String(json);
	ASSERT_EQ(result.features.size(), 1);
	
	auto* line_string = dynamic_cast<GeoJSON::Line_String*>(result.features[0].geometry.get());
	ASSERT_NE(line_string, nullptr);
	EXPECT_EQ(line_string->positions.size(), points.size());
	
	for (size_t i = 0; i < points.size(); ++i) {
		auto [expected_lon, expected_lat] = points[i];
		EXPECT_NEAR(line_string->positions[i].longitude, expected_lon, 1e-10);
		EXPECT_NEAR(line_string->positions[i].latitude, expected_lat, 1e-10);
	}
}

INSTANTIATE_TEST_SUITE_P(
	Line_String_Coordinates,
	Line_String_Geometry_Test,
	::testing::Values(
		std::make_tuple("Simple line",
			std::vector<std::tuple<double, double>>{{100.0, 0.0}, {101.0, 1.0}}),
		std::make_tuple("Three point line",
			std::vector<std::tuple<double, double>>{{0.0, 0.0}, {1.0, 1.0}, {2.0, 2.0}}),
		std::make_tuple("Complex line",
			std::vector<std::tuple<double, double>>{
				{100.0, 0.0}, {101.0, 0.0}, {101.0, 1.0}, {100.0, 1.0}, {100.0, 0.0}
			})
	)
);
