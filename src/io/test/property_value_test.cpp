#include "property_value_test.h"


TEST_P(Property_Value_Test, Parse_Different_Property_Types)
{
	auto [value_type, json_value, expected_accessor] = GetParam();
	
	std::string json = std::format(R"({{
		"type": "Feature",
		"geometry": {{
			"type": "Point",
			"coordinates": [0, 0]
		}},
		"properties": {{
			"test_value": {}
		}}
	}})", json_value);

	auto result = IO::Parse_Geojson_String(json);
	ASSERT_EQ(result.features.size(), 1);
	ASSERT_FALSE(result.features[0].properties.empty());
	
	auto it = result.features[0].properties.find("test_value");
	ASSERT_NE(it, result.features[0].properties.end());
	
	// Test the appropriate type
	if (expected_accessor == "string")
		EXPECT_TRUE(it->second.is_string());
	else if (expected_accessor == "number")
		EXPECT_TRUE(it->second.is_number());
	else if (expected_accessor == "bool")
		EXPECT_TRUE(it->second.is_bool());
	else if (expected_accessor == "null")
		EXPECT_TRUE(it->second.is_null());
}

INSTANTIATE_TEST_SUITE_P(
	Property_Types,
	Property_Value_Test,
	::testing::Values(
		std::make_tuple("String", "\"hello world\"", "string"),
		std::make_tuple("Number", "42.5", "number"),
		std::make_tuple("Integer", "100", "number"),
		std::make_tuple("Boolean true", "true", "bool"),
		std::make_tuple("Boolean false", "false", "bool"),
		std::make_tuple("Null", "null", "null"),
		std::make_tuple("Negative number", "-15.75", "number")
	)
);
