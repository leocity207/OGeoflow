#include "coordination_validation_test.h"

#include <format>
#include "include/io/exception.h"

TEST_P(Coordinate_Validation_Test, Validate_Coordinate_Ranges) {
	auto [lon, lat, should_pass] = GetParam();
	
	std::string json = std::format(R"({{
		"type": "Feature",
		"geometry": {{
			"type": "Point",
			"coordinates": [{}, {}]
		}}
	}})", lon, lat);

	if (should_pass) {
		EXPECT_NO_THROW(IO::Parse_Geojson_String(json));
	} else {
		EXPECT_THROW(IO::Parse_Geojson_String(json), IO::Exception);
	}
}

INSTANTIATE_TEST_SUITE_P(
	Coordinate_Ranges,
	Coordinate_Validation_Test,
	::testing::Values(
		std::make_tuple(0.0, 0.0, true),
		std::make_tuple(-180.0, -90.0, true),
		std::make_tuple(180.0, 90.0, true),
		std::make_tuple(-181.0, 0.0, false),
		std::make_tuple(181.0, 0.0, false),
		std::make_tuple(0.0, -91.0, false),
		std::make_tuple(0.0, 91.0, false),
		std::make_tuple(-180.0, -90.0, true)  // Edge case
	)
);
