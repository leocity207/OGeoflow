#include "parser_test.h"

#include "include/io/exception.h"
#include "include/geojson/geometry/geometry_collection.h"

// Test basic GeoJSON types
TEST_F(GeoJSON_Parser_Test, ParseFeatureCollection) {
	std::string json = R"({
		"type": "FeatureCollection",
		"features": []
	})";

	auto result = IO::Parse_Geojson_String(json);
	EXPECT_EQ(result.type, GeoJSON::GeoJSON::Type::FEATURE_COLLECTION);
	EXPECT_TRUE(result.features.empty());
}

TEST_F(GeoJSON_Parser_Test, ParseSingleFeature) {
	std::string json = R"({
		"type": "Feature",
		"geometry": {
			"type": "Point",
			"coordinates": [100.0, 0.0]
		},
		"properties": {
			"name": "Test Feature"
		}
	})";

	auto result = IO::Parse_Geojson_String(json);
	EXPECT_EQ(result.type, GeoJSON::GeoJSON::Type::FEATURE);
	EXPECT_EQ(result.features.size(), 1);
}

// Error handling tests
TEST_F(GeoJSON_Parser_Test, InvalidJSONThrowsException) {
	std::string invalid_json = R"({
		"type": "FeatureCollection",
		"features": [
	})";  // Missing closing bracket

	EXPECT_THROW(IO::Parse_Geojson_String(invalid_json), IO::Exception);
}

TEST_F(GeoJSON_Parser_Test, InvalidGeoJSONTypeThrowsException) {
	std::string json = R"({
		"type": "InvalidType",
		"features": []
	})";

	EXPECT_THROW(IO::Parse_Geojson_String(json), IO::Exception);
}

TEST_F(GeoJSON_Parser_Test, PointWithInsufficientCoordinatesThrows) {
	std::string json = R"({
		"type": "Feature",
		"geometry": {
			"type": "Point",
			"coordinates": [100.0]
		}
	})";

	EXPECT_THROW(IO::Parse_Geojson_String(json), IO::Exception);
}

TEST_F(GeoJSON_Parser_Test, PointWithInvalidCoordinateTypeThrows) {
	std::string json = R"({
		"type": "Feature",
		"geometry": {
			"type": "Point",
			"coordinates": ["100.0", 0.0]
		}
	})";

	EXPECT_THROW(IO::Parse_Geojson_String(json), IO::Exception);
}

TEST_F(GeoJSON_Parser_Test, LineStringWithSinglePointThrows) {
	std::string json = R"({
		"type": "Feature",
		"geometry": {
			"type": "LineString",
			"coordinates": [[100.0, 0.0]]
		}
	})";

	EXPECT_THROW(IO::Parse_Geojson_String(json), IO::Exception);
}

// BBOX tests
TEST_F(GeoJSON_Parser_Test, ParseBBox) {
	std::string json = R"({
		"type": "FeatureCollection",
		"bbox": [-180.0, -90.0, 180.0, 90.0],
		"features": []
	})";

	auto result = IO::Parse_Geojson_String(json);
	EXPECT_TRUE(result.bbox.has_value());
	ASSERT_EQ(result.bbox->size(), 4);
	EXPECT_DOUBLE_EQ(result.bbox->at(0), -180.0);
	EXPECT_DOUBLE_EQ(result.bbox->at(1), -90.0);
	EXPECT_DOUBLE_EQ(result.bbox->at(2), 180.0);
	EXPECT_DOUBLE_EQ(result.bbox->at(3), 90.0);
}

TEST_F(GeoJSON_Parser_Test, ParseBBoxWithAltitude) {
	std::string json = R"({
		"type": "FeatureCollection",
		"bbox": [-180.0, -90.0, -1000.0, 180.0, 90.0, 5000.0],
		"features": []
	})";

	auto result = IO::Parse_Geojson_String(json);
	EXPECT_TRUE(result.bbox.has_value());
	ASSERT_EQ(result.bbox->size(), 6);
	EXPECT_DOUBLE_EQ(result.bbox->at(4), 5000.0);
}

// Feature ID tests
TEST_F(GeoJSON_Parser_Test, ParseFeatureWithStringId) {
	std::string json = R"({
		"type": "Feature",
		"id": "feature-123",
		"geometry": {
			"type": "Point",
			"coordinates": [0, 0]
		}
	})";

	auto result = IO::Parse_Geojson_String(json);
	ASSERT_EQ(result.features.size(), 1);
	EXPECT_EQ(result.features[0].id, "feature-123");
}

TEST_F(GeoJSON_Parser_Test, ParseFeatureWithNumericId) {
	std::string json = R"({
		"type": "Feature",
		"id": 12345,
		"geometry": {
			"type": "Point",
			"coordinates": [0, 0]
		}
	})";

	auto result = IO::Parse_Geojson_String(json);
	ASSERT_EQ(result.features.size(), 1);
	EXPECT_EQ(result.features[0].id, "12345");
}

// GeometryCollection tests
TEST_F(GeoJSON_Parser_Test, ParseGeometryCollection) {
	std::string json = R"({
		"type": "GeometryCollection",
		"geometries": [
			{
				"type": "Point",
				"coordinates": [100.0, 0.0]
			},
			{
				"type": "LineString",
				"coordinates": [[101.0, 0.0], [102.0, 1.0]]
			}
		]
	})";

	auto result = IO::Parse_Geojson_String(json);
	EXPECT_EQ(result.type, GeoJSON::GeoJSON::Type::GEOMETRY);
	ASSERT_NE(result.geometry, nullptr);
	
	auto* collection = dynamic_cast<GeoJSON::Geometry_Collection*>(result.geometry.get());
	ASSERT_NE(collection, nullptr);
	EXPECT_EQ(collection->geometries.size(), 2);
}

// Performance test for large FeatureCollection
TEST_F(GeoJSON_Parser_Test, LargeFeatureCollection) {
	const int num_features = 1000;
	std::stringstream json;
	json << R"({"type": "FeatureCollection", "features": [)";
	
	for (int i = 0; i < num_features; ++i) {
		json << R"({"type": "Feature", "geometry": {"type": "Point", "coordinates": [)"
			 << i << ", " << i << R"(]}, "properties": {"id": )" << i << "}}";
		if (i < num_features - 1) json << ",";
	}
	json << "]}";

	auto start_time = std::chrono::high_resolution_clock::now();
	auto result = IO::Parse_Geojson_String(json.str());
	auto end_time = std::chrono::high_resolution_clock::now();
	
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	
	EXPECT_EQ(result.features.size(), num_features);
	EXPECT_LT(duration.count(), 1000); // Should parse 1000 features in less than 1 second
	
	// Verify a few features
	EXPECT_EQ(result.features[0].properties["id"].as_number(), 0);
	EXPECT_EQ(result.features[999].properties["id"].as_number(), 999);
}

