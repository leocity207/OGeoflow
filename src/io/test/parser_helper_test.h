#ifndef SRC_IO_TEST_PARSER_HELPER_TEST_H
#define SRC_IO_TEST_PARSER_HELPER_TEST_H

#include <string>
#include <vector>
#include <optional>

#include <gtest/gtest.h>

#include "include/io/parser.h"

struct ExpectedPos
{
    double lon;
    double lat;
    std::optional<double> alt;
};

inline GeoJSON::Geometry Parse_To_Geometry(const std::string& json)
{
	auto result = IO::Parse_Geojson_String(json);
	EXPECT_TRUE(result.Has_Value()) << "Parse failed: error = " << static_cast<int>(result.Error());
	auto& g = result.Value();
	EXPECT_TRUE(g.Is_Geometry());
	return g.Get_Geometry();
}

// assert parse error
inline void Expect_Parse_Error(const std::string& json, IO::Error::Type expected_error)
{
	auto result = IO::Parse_Geojson_String(json);
	EXPECT_FALSE(result.Has_Value());
	EXPECT_EQ(result.Error(), expected_error);
}

// compare one Position to ExpectedPos (use double eq)
inline void AssertPositionEquals(const GeoJSON::Position& p, const ExpectedPos& e)
{
	EXPECT_DOUBLE_EQ(p.longitude, e.lon);
	EXPECT_DOUBLE_EQ(p.latitude, e.lat);
	if (e.alt.has_value()) {
		ASSERT_TRUE(p.altitude.has_value());
		EXPECT_DOUBLE_EQ(*p.altitude, *e.alt);
	} else {
		EXPECT_FALSE(p.altitude.has_value());
	}
}

// compare a sequence of positions with expected vector
template <typename Container>
inline void AssertPositionsEquals(const Container& positions, const std::vector<ExpectedPos>& expected)
{
	ASSERT_EQ(positions.size(), expected.size());
	for (size_t i = 0; i < expected.size(); ++i) {
		AssertPositionEquals(positions[i], expected[i]);
	}
}

// helper pour récupérer une clé d'objet et échouer si absente / pas objet
inline const GeoJSON::Property& GetObjectMemberOrFail(const GeoJSON::Property& objProp, const std::string& key)
{
	EXPECT_TRUE(objProp.Is_Object()) << "Expected object property";
    const auto& m = objProp.Get_Object();
    auto it = m.find(key);
	EXPECT_TRUE(it != m.end()) << "Key '" << key << "' not found in object";
    return it->second;
}

// helper pour récupérer un élément de tableau et échouer si pas tableau
inline const GeoJSON::Property& GetArrayElementOrFail(const GeoJSON::Property& arrProp, std::size_t idx)
{
	EXPECT_TRUE(arrProp.Is_Array()) << "Expected array property";
    const auto& a = arrProp.Get_Array();
	EXPECT_LT(idx, a.size()) << "Index out of range";
    return a[idx];
}

inline const GeoJSON::Feature& FirstFeatureOrFail(const GeoJSON::GeoJSON& g)
{
	EXPECT_TRUE(g.Is_Feature_Collection()) << "Expected FeatureCollection";
    const auto& fc = g.Get_Feature_Collection();
	EXPECT_FALSE(fc.features.empty()) << "FeatureCollection has no features";
    return fc.features[0];
}

#endif //SRC_IO_TEST_PARSER_HELPER_TEST_H