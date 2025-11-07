#include "property_test.h"

#include "include/io/parser.h"
#include "parser_helper_test.h"

TEST_F(Property_Test, Properties_Primitives_And_Null) {
	std::string json = R"({
		"type": "Feature",
		"geometry": { "type": "Point", "coordinates": [0.0, 0.0] },
		"properties": {
			"s": "hello",
			"i": 42,
			"d": 3.14,
			"b": true,
			"n": null
		}
	})";

	auto result = GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parse failed: error=" << static_cast<int>(result.Error());
	const auto& f = result.Value().Get_Feature();

	ASSERT_TRUE(f.properties.Is_Object());
	const auto& obj = f.properties.Get_Object();

	ASSERT_TRUE(obj.find("s") != obj.end());
	EXPECT_TRUE(obj.at("s").Is_String());
	EXPECT_EQ(obj.at("s").Get_String(), "hello");

	ASSERT_TRUE(obj.find("i") != obj.end());
	if (obj.at("i").Is_Integer()) {
		EXPECT_EQ(obj.at("i").Get_Int(), 42);
	} else {
		EXPECT_TRUE(obj.at("i").Is_Double());
		EXPECT_DOUBLE_EQ(obj.at("i").Get_Double(), 42.0);
	}

	ASSERT_TRUE(obj.find("d") != obj.end());
	if (obj.at("d").Is_Double()) {
		EXPECT_NEAR(obj.at("d").Get_Double(), 3.14, 1e-9);
	} else {
		EXPECT_TRUE(obj.at("d").Is_Integer());
		EXPECT_EQ(obj.at("d").Get_Int(), 3); // if stored as int (unlikely), accept coerce
	}

	ASSERT_TRUE(obj.find("b") != obj.end());
	EXPECT_TRUE(obj.at("b").Is_Bool());
	EXPECT_TRUE(obj.at("b").Get_Bool());

	ASSERT_TRUE(obj.find("n") != obj.end());
	EXPECT_TRUE(obj.at("n").Is_Null());
}

TEST_F(Property_Test, Properties_Array_Of_Primitives) {
	std::string json = R"({
		"type": "Feature",
		"geometry": { "type": "Point", "coordinates": [0,0] },
		"properties": {
			"arr": [1, "two", null, false, 3.5]
		}
	})";

	auto result = GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value());
	const auto& f = result.Value().Get_Feature();

	const auto& arrProp = GetObjectMemberOrFail(f.properties, "arr");
	ASSERT_TRUE(arrProp.Is_Array());
	const auto& arr = arrProp.Get_Array();
	ASSERT_EQ(arr.size(), 5u);

	if (arr[0].Is_Integer()) EXPECT_EQ(arr[0].Get_Int(), 1);
	else EXPECT_DOUBLE_EQ(arr[0].Get_Double(), 1.0);

	EXPECT_TRUE(arr[1].Is_String());
	EXPECT_EQ(arr[1].Get_String(), "two");

	EXPECT_TRUE(arr[2].Is_Null());

	EXPECT_TRUE(arr[3].Is_Bool());
	EXPECT_FALSE(arr[3].Get_Bool());

	if (arr[4].Is_Double()) EXPECT_NEAR(arr[4].Get_Double(), 3.5, 1e-9);
	else EXPECT_EQ(arr[4].Get_Int(), 3);
}

TEST_F(Property_Test, Properties_Nested_Object) {
	std::string json = R"({
		"type": "Feature",
		"geometry": { "type": "Point", "coordinates": [0,0] },
		"properties": {
			"outer": {
				"inner": {
					"value": 123
				}
			}
		}
	})";

	auto result = GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value());
	const auto& f = result.Value().Get_Feature();

	const auto& outer = GetObjectMemberOrFail(f.properties, "outer");
	const auto& inner = GetObjectMemberOrFail(outer, "inner");
	ASSERT_TRUE(inner.Is_Object());
	const auto& inner_map = inner.Get_Object();
	ASSERT_TRUE(inner_map.find("value") != inner_map.end());
	if (inner_map.at("value").Is_Integer())
		EXPECT_EQ(inner_map.at("value").Get_Int(), 123);
	else
		EXPECT_DOUBLE_EQ(inner_map.at("value").Get_Double(), 123.0);
}

TEST_F(Property_Test, Properties_Complex_Mix_Object_Array_Object) {
	std::string json = R"({
		"type": "Feature",
		"geometry": { "type": "Point", "coordinates": [0,0] },
		"properties": {
			"mixed": [
				{ "k": "v", "inner": [1, 2, { "deep": 5 }] },
				[ "x", null, { "deep2": [true, false] } ],
				[]
			],
			"empty_obj": {},
			"empty_arr": []
		}
	})";

	auto result = GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value());
	const auto& f = result.Value().Get_Feature();

	// mixed
	const auto& mixedProp = GetObjectMemberOrFail(f.properties, "mixed");
	ASSERT_TRUE(mixedProp.Is_Array());
	const auto& mixedArr = mixedProp.Get_Array();
	ASSERT_EQ(mixedArr.size(), 3u);

	// mixedArr[0] is object with k and inner array
	ASSERT_TRUE(mixedArr[0].Is_Object());
	{
		const auto& m0 = mixedArr[0].Get_Object();
		ASSERT_TRUE(m0.find("k") != m0.end());
		EXPECT_TRUE(m0.at("k").Is_String());
		EXPECT_EQ(m0.at("k").Get_String(), "v");

		ASSERT_TRUE(m0.find("inner") != m0.end());
		EXPECT_TRUE(m0.at("inner").Is_Array());
		const auto& innerArr = m0.at("inner").Get_Array();
		ASSERT_EQ(innerArr.size(), 3u);
		// innerArr[0] == 1
		if (innerArr[0].Is_Integer()) EXPECT_EQ(innerArr[0].Get_Int(), 1);
		else EXPECT_DOUBLE_EQ(innerArr[0].Get_Double(), 1.0);
		// innerArr[2] is object {"deep":5}
		ASSERT_TRUE(innerArr[2].Is_Object());
		const auto& deep = innerArr[2].Get_Object();
		ASSERT_TRUE(deep.find("deep") != deep.end());
		if (deep.at("deep").Is_Integer()) EXPECT_EQ(deep.at("deep").Get_Int(), 5);
		else EXPECT_DOUBLE_EQ(deep.at("deep").Get_Double(), 5.0);
	}

	// mixedArr[1] is an array
	ASSERT_TRUE(mixedArr[1].Is_Array());
	const auto& m1 = mixedArr[1].Get_Array();
	ASSERT_EQ(m1.size(), 3u);
	EXPECT_TRUE(m1[0].Is_String());
	EXPECT_TRUE(m1[1].Is_Null());
	ASSERT_TRUE(m1[2].Is_Object());
	{
		const auto& deep2 = m1[2].Get_Object();
		ASSERT_TRUE(deep2.find("deep2") != deep2.end());
		EXPECT_TRUE(deep2.at("deep2").Is_Array());
		const auto& deep2arr = deep2.at("deep2").Get_Array();
		ASSERT_EQ(deep2arr.size(), 2u);
		EXPECT_TRUE(deep2arr[0].Is_Bool());
		EXPECT_TRUE(deep2arr[1].Is_Bool());
	}

	// empty_obj
	const auto& emptyObj = GetObjectMemberOrFail(f.properties, "empty_obj");
	ASSERT_TRUE(emptyObj.Is_Object());
	EXPECT_TRUE(emptyObj.Get_Object().empty());

	// empty_arr
	const auto& emptyArr = GetObjectMemberOrFail(f.properties, "empty_arr");
	ASSERT_TRUE(emptyArr.Is_Array());
	EXPECT_TRUE(emptyArr.Get_Array().empty());
}

TEST_F(Property_Test, Properties_Explicitly_Null) {
	std::string json = R"({
		"type": "Feature",
		"geometry": { "type": "Point", "coordinates": [0,0] },
		"properties": null
	})";

	auto result = GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value()) << "Parse failed: error=" << static_cast<int>(result.Error());
	const auto& f = result.Value().Get_Feature();

	if (f.properties.Is_Null())
		SUCCEED() << "properties parsed as explicit null";
	else if (f.properties.Is_Object())
		SUCCEED() << "properties coerced to empty object";
	else
		FAIL() << "properties parsed into unexpected type";
}

TEST_F(Property_Test, Properties_Keys_Preservation) {
	std::string json = R"({
		"type": "Feature",
		"geometry": { "type": "Point", "coordinates": [0,0] },
		"properties": {
			"first": 1,
			"second": 2,
			"third": { "a": 10 }
		}
	})";

	auto result = GeoJSON::IO::Parse_Geojson_String(json);
	ASSERT_TRUE(result.Has_Value());
	const auto& f = result.Value().Get_Feature();

	ASSERT_TRUE(f.properties.Is_Object());
	const auto& obj = f.properties.Get_Object();
	EXPECT_TRUE(obj.find("first") != obj.end());
	EXPECT_TRUE(obj.find("second") != obj.end());
	EXPECT_TRUE(obj.find("third") != obj.end());
	EXPECT_TRUE(obj.at("first").Is_Integer() || obj.at("first").Is_Double());
	EXPECT_TRUE(obj.at("second").Is_Integer() || obj.at("second").Is_Double());
	EXPECT_TRUE(obj.at("third").Is_Object());
}

TEST_F(Property_Test, Properties_Malformed_Inconsistent_Array_Object) {
	std::string json = R"({
		"type": "Feature",
		"geometry": { "type": "Point", "coordinates": [0,0] },
		"properties": {
			"x": [],
			"x": { "sub": 1 }
		}
	})";

	auto result = GeoJSON::IO::Parse_Geojson_String(json);
	EXPECT_EQ(result.Error(), GeoJSON::IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST);

}