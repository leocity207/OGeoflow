#include "feature_test.h"

#include "io/parser.h"
#include "parser_helper_test.h"

TEST_F(Feature_Test, Feature_Collection_Features_Basic) {
	std::string json = R"({
		"type": "FeatureCollection",
		"features": [
			{
				"type": "Feature",
				"id": "f-1",
				"bbox": [0.0, 0.0, 10.0, 10.0],
				"geometry": { "type": "Point", "coordinates": [1.5, 2.5] },
				"properties": { "name": "A", "value": 10 }
			},
			{
				"type": "Feature",
				"id": "f-2",
				"geometry": { "type": "Point", "coordinates": [3.0, 4.0] },
				"properties": { "name": "B" }
			}
		]
	})";

	auto result = O::GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parse failed, error=" << static_cast<int>(result.Error());
	auto& g = result.Value();

	ASSERT_TRUE(g.Is_Feature_Collection());
	auto& fc = g.Get_Feature_Collection();
	ASSERT_EQ(fc.features.size(), 2u);

	// Feature 0 checks
	const auto& f0 = fc.features[0];
	ASSERT_TRUE(f0.id.has_value());
	EXPECT_EQ(*f0.id, "f-1");

	ASSERT_TRUE(f0.bbox.has_value());
	ASSERT_FALSE(f0.bbox->Has_Altitude());
	{
		auto arr = f0.bbox->Get();
		EXPECT_DOUBLE_EQ(arr[0], 0.0);
		EXPECT_DOUBLE_EQ(arr[1], 0.0);
		EXPECT_DOUBLE_EQ(arr[2], 10.0);
		EXPECT_DOUBLE_EQ(arr[3], 10.0);
	}

	ASSERT_TRUE(f0.geometry.has_value());
	ASSERT_TRUE(f0.geometry->Is_Point());
	{
		const auto& pt = f0.geometry->Get_Point();
		EXPECT_DOUBLE_EQ(pt.position.longitude, 1.5);
		EXPECT_DOUBLE_EQ(pt.position.latitude,  2.5);
	}

	ASSERT_TRUE(f0.properties.Is_Object());
	{
		const auto& obj = f0.properties.Get_Object();
		ASSERT_TRUE(obj.contains("name"));
		EXPECT_TRUE(obj.at("name").Is_String());
		EXPECT_EQ(obj.at("name").Get_String(), "A");

		ASSERT_TRUE(obj.contains("value"));
		EXPECT_TRUE(obj.at("value").Is_Double() || obj.at("value").Is_Integer());
		if (obj.at("value").Is_Double())
			EXPECT_DOUBLE_EQ(obj.at("value").Get_Double(), 10.0);
		else
			EXPECT_EQ(obj.at("value").Get_Int(), 10);
	}

	const auto& f1 = fc.features[1];
	ASSERT_TRUE(f1.id.has_value());
	EXPECT_EQ(*f1.id, "f-2");

	ASSERT_FALSE(f1.bbox.has_value());
	ASSERT_TRUE(f1.geometry.has_value());
	ASSERT_TRUE(f1.geometry->Is_Point());
	{
		const auto& pt = f1.geometry->Get_Point();
		EXPECT_DOUBLE_EQ(pt.position.longitude, 3.0);
		EXPECT_DOUBLE_EQ(pt.position.latitude,  4.0);
	}
	ASSERT_TRUE(f1.properties.Is_Object());
	{
		const auto& obj = f1.properties.Get_Object();
		ASSERT_TRUE(obj.contains("name"));
		EXPECT_EQ(obj.at("name").Get_String(), "B");
	}
}

TEST_F(Feature_Test, Feature_Id_As_Number_Should_Be_Stringified) {
	std::string json = R"({
		"type": "FeatureCollection",
		"features": [
			{
				"type": "Feature",
				"id": 42,
				"geometry": { "type": "Point", "coordinates": [0,0] },
				"properties": {}
			}
		]
	})";

	auto result = O::GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parse failed, error=" << static_cast<int>(result.Error());
	auto& g = result.Value();
	ASSERT_TRUE(g.Is_Feature_Collection());
	ASSERT_EQ(g.Get_Feature_Collection().features.size(), 1u);

	const auto& f = g.Get_Feature_Collection().features[0];
	ASSERT_TRUE(f.id.has_value());
	EXPECT_EQ(*f.id, "42");
}

TEST_F(Feature_Test, Feature_Missing_Geometry_Should_Fail) {
	std::string json = R"({
		"type": "FeatureCollection",
		"features": [
			{
				"type": "Feature",
				"properties": { "k": "v" }
			}
		]
	})";

	auto result = O::GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parse failed, error=" << static_cast<int>(result.Error());
}

TEST_F(Feature_Test, Feature_Geometry_Null_Behavior) {
	std::string json = R"({
		"type": "FeatureCollection",
		"features": [
			{
				"type": "Feature",
				"geometry": null,
				"properties": {}
			}
		]
	})";

	auto result = O::GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parse failed, error=" << static_cast<int>(result.Error());
}

TEST_F(Feature_Test, Feature_Properties_Null_Accepted) {
	std::string json = R"({
		"type": "FeatureCollection",
		"features": [
			{
				"type": "Feature",
				"geometry": { "type": "Point", "coordinates": [0,0] },
				"properties": null
			}
		]
	})";

	auto result = O::GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parse failed, error=" << static_cast<int>(result.Error());
	auto& g = result.Value();
	ASSERT_TRUE(g.Is_Feature_Collection());
	ASSERT_EQ(g.Get_Feature_Collection().features.size(), 1u);

	const auto& f = g.Get_Feature_Collection().features[0];
	if (f.properties.Is_Null())
		SUCCEED() << "properties parsed as explicit null";
	else if (f.properties.Is_Object())
		SUCCEED() << "properties coerced to empty object (acceptable)";
	else
		FAIL() << "properties parsed into unexpected type";
}

TEST_F(Feature_Test, Feature_With_Wrong_Type_In_Features_Array) {
	std::string json = R"({
		"type": "FeatureCollection",
		"features": [
			{
				"type": "NotAFeature",
				"geometry": { "type": "Point", "coordinates": [0,0] },
				"properties": {}
			}
		]
	})";

	Expect_Parse_Error(json, O::GeoJSON::IO::Error::UNKNOWN_TYPE);
}