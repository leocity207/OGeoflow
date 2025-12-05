#include "root_test.h"

#include "io/parser.h"
#include "parser_helper_test.h"

TEST_F(Root_Test, Root_Is_Geometry_Point) {
	std::string json = R"({
		"type": "Point",
		"coordinates": [12.34, 56.78]
	})";

	auto result = O::GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parsing failed: error=" << static_cast<int>(result.Error());
	auto& g = result.Value();

	EXPECT_TRUE(g.Is_Geometry());
	EXPECT_FALSE(g.Is_Feature());
	EXPECT_FALSE(g.Is_Feature_Collection());

	auto& geom = g.Get_Geometry();
	EXPECT_TRUE(geom.Is_Point());
	const auto& pt = geom.Get_Point();
	EXPECT_DOUBLE_EQ(pt.position.longitude, 12.34);
	EXPECT_DOUBLE_EQ(pt.position.latitude, 56.78);
	EXPECT_FALSE(pt.position.altitude.has_value());
}

TEST_F(Root_Test, Root_Is_Feature_Basic) {
	std::string json = R"({
		"type": "Feature",
		"geometry": {
			"type": "Point",
			"coordinates": [1.0, 2.0]
		},
		"properties": {
			"name": "test"
		}
	})";

	auto result = O::GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parsing failed: error=" << static_cast<int>(result.Error());
	auto& g = result.Value();

	EXPECT_TRUE(g.Is_Feature());
	EXPECT_FALSE(g.Is_Geometry());
	EXPECT_FALSE(g.Is_Feature_Collection());

	auto& feature = g.Get_Feature();
	ASSERT_TRUE(feature.geometry.has_value());
	EXPECT_TRUE(feature.geometry->Is_Point());
	EXPECT_TRUE(feature.properties.Is_Object());
	const auto& pt = feature.geometry->Get_Point();
	EXPECT_DOUBLE_EQ(pt.position.longitude, 1.0);
	EXPECT_DOUBLE_EQ(pt.position.latitude, 2.0);
}

TEST_F(Root_Test, Feature_With_Id_And_BBox) {
	std::string json = R"({
		"type": "Feature",
		"id": "feature-42",
		"bbox": [100.0, 0.0, 110.0, 10.0],
		"geometry": {
			"type": "Point",
			"coordinates": [10.0, 20.0]
		},
		"properties": {}
	})";

	auto result = O::GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parsing failed: error=" << static_cast<int>(result.Error());
	auto& g = result.Value();

	ASSERT_TRUE(g.Is_Feature());
	auto& feature = g.Get_Feature();

	ASSERT_TRUE(feature.id.has_value());
	EXPECT_EQ(*feature.id, "feature-42");

	ASSERT_TRUE(feature.bbox.has_value());
	if (std::holds_alternative<std::array<double,4>>(feature.bbox->coordinates)) {
		auto arr = std::get<std::array<double,4>>(feature.bbox->coordinates);
		EXPECT_DOUBLE_EQ(arr[0], 100.0);
		EXPECT_DOUBLE_EQ(arr[1], 0.0);
		EXPECT_DOUBLE_EQ(arr[2], 110.0);
		EXPECT_DOUBLE_EQ(arr[3], 10.0);
	} else 
		FAIL() << "Expected 4-values bbox";
}

TEST_F(Root_Test, Feature_Collection_With_BBox) {
	std::string json = R"({
		"type": "FeatureCollection",
		"bbox": [0.0, 0.0, 10.0, 10.0],
		"features": [
			{
				"type": "Feature",
				"geometry": { "type": "Point", "coordinates": [0.0, 0.0] },
				"properties": {}
			},
			{
				"type": "Feature",
				"geometry": { "type": "Point", "coordinates": [1.0, 1.0] },
				"properties": {}
			}
		]
	})";

	auto result = O::GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parsing failed: error=" << static_cast<int>(result.Error());
	auto& g = result.Value();

	EXPECT_TRUE(g.Is_Feature_Collection());
	EXPECT_FALSE(g.Is_Feature());
	EXPECT_FALSE(g.Is_Geometry());

	ASSERT_TRUE(g.Get_Feature_Collection().bbox.has_value());
	if (std::holds_alternative<std::array<double,4>>(g.Get_Feature_Collection().bbox->coordinates)) {
		auto arr = std::get<std::array<double,4>>(g.Get_Feature_Collection().bbox->coordinates);
		EXPECT_DOUBLE_EQ(arr[0], 0.0);
		EXPECT_DOUBLE_EQ(arr[1], 0.0);
		EXPECT_DOUBLE_EQ(arr[2], 10.0);
		EXPECT_DOUBLE_EQ(arr[3], 10.0);
	} else {
		FAIL() << "Expected 4-values bbox at root";
	}

	auto& fc = g.Get_Feature_Collection();
	EXPECT_EQ(fc.features.size(), 2u);
	ASSERT_TRUE(fc.features[0].geometry.has_value());
	ASSERT_TRUE(fc.features[1].geometry.has_value());
	EXPECT_TRUE(fc.features[0].geometry->Is_Point());
	EXPECT_TRUE(fc.features[1].geometry->Is_Point());
}

TEST_F(Root_Test, Root_With_Wrong_Type_String_Fails) {
	std::string json = R"({
		"type": "NotARealGeoJSONType"
	})";

	Expect_Parse_Error(json, O::GeoJSON::IO::Error::UNKNOWN_TYPE);
}
