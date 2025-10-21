#include "foreign_test.h"

#include "include/io/parser.h"
#include "parser_helper_test.h"

TEST_F(Foreign_Test, Root_Foreign_Primitives_And_Null_Do_Not_Break_Parse) {
    std::string json = R"({
        "type": "Point",
        "coordinates": [12.0, 34.0],
        "x-custom-int": 123,
        "x-custom-str": "hello",
        "x-custom-bool": true,
        "x-custom-null": null
    })";

    auto result = IO::Parse_Geojson_String(json);
    ASSERT_TRUE(result.Has_Value()) << "Parser failed on root foreign primitives";
    auto& g = result.Value();
    ASSERT_TRUE(g.Is_Geometry());
    auto& geom = g.Get_Geometry();
    ASSERT_TRUE(geom.Is_Point());
    AssertPositionEquals(geom.Get_Point().position, ExpectedPos{12.0, 34.0, std::nullopt});
}

TEST_F(Foreign_Test, Root_Foreign_Object_And_Array_Nested_Do_Not_Affect_Main) {
    std::string json = R"({
        "type": "Point",
        "coordinates": [1.0, 2.0],
        "x-complex": {
            "a": [1, 2, {"deep": [true, false, null]}],
            "b": { "inner": [ [1,2], {"k":"v"} ] }
        }
    })";

    auto result = IO::Parse_Geojson_String(json);
    ASSERT_TRUE(result.Has_Value()) << "Parser failed on root nested foreign object";
    auto& g = result.Value();
    ASSERT_TRUE(g.Is_Geometry());
    ASSERT_TRUE(g.Get_Geometry().Is_Point());
    AssertPositionEquals(g.Get_Geometry().Get_Point().position, ExpectedPos{1.0, 2.0, std::nullopt});
}

TEST_F(Foreign_Test, Feature_Level_Foreign_Key_Mixed_Types) {
    std::string json = R"({
        "type": "Feature",
        "id": "f0",
        "geometry": { "type": "Point", "coordinates": [7.0, 8.0] },
        "properties": { "name": "core" },
        "x-meta": { "notes": [ "a", null, 3 ] },
        "x-flag": false
    })";

    auto result = IO::Parse_Geojson_String(json);
    ASSERT_TRUE(result.Has_Value()) << "Parser failed on feature-level foreign keys";
    auto& g = result.Value();
    ASSERT_TRUE(g.Is_Feature());
    const auto& feature = g.Get_Feature();
    // core fields unaffected:
    ASSERT_TRUE(feature.id.has_value());
    EXPECT_EQ(*feature.id, "f0");
    EXPECT_TRUE(feature.geometry.Is_Point());
    AssertPositionEquals(feature.geometry.Get_Point().position, ExpectedPos{7.0, 8.0, std::nullopt});
    ASSERT_TRUE(feature.properties.Is_Object());
    const auto& props = feature.properties.Get_Object();
    ASSERT_TRUE(props.find("name") != props.end());
    EXPECT_EQ(props.at("name").Get_String(), "core");
}

TEST_F(Foreign_Test, Geometry_Level_Foreign_Key_Does_Not_Break_Geometry) {
    std::string json = R"({
        "type": "Point",
        "coordinates": [5, 6],
        "crs": { "type": "name", "properties": { "name": "EPSG:4326" } }, 
        "x-geom": [ { "a":1 }, { "b":[2,3] }, null ]
    })";

    auto result = IO::Parse_Geojson_String(json);
    ASSERT_TRUE(result.Has_Value()) << "Parser failed on geometry-level foreign keys";
    auto& g = result.Value();
    ASSERT_TRUE(g.Is_Geometry());
    AssertPositionEquals(g.Get_Geometry().Get_Point().position, ExpectedPos{5.0, 6.0, std::nullopt});
}

TEST_F(Foreign_Test, Root_Foreign_Array_Complex_Elements) {
    std::string json = R"({
        "type": "LineString",
        "coordinates": [[0,0],[1,1]],
        "x-rows": [
            { "meta": [1,2,3] },
            [ "a", {"inner": null}, [1,2] ],
            null,
            42
        ]
    })";

    auto result = IO::Parse_Geojson_String(json);
    ASSERT_TRUE(result.Has_Value()) << "Parser failed on foreign array with complex elements";
    ASSERT_TRUE(result.Value().Is_Geometry());
    ASSERT_TRUE(result.Value().Get_Geometry().Is_Line_String());
    const auto& line = result.Value().Get_Geometry().Get_Line_String();
    ASSERT_GE(line.positions.size(), 2u);
    AssertPositionsEquals(line.positions, std::vector<ExpectedPos>{ {0,0,std::nullopt}, {1,1,std::nullopt} });
}

TEST_F(Foreign_Test, Feature_Collection_Multiple_Features_With_Foreign_Keys) {
    std::string json = R"({
        "type": "FeatureCollection",
        "features": [
            {
                "type": "Feature",
                "geometry": { "type": "Point", "coordinates": [0,0] },
                "properties": {},
                "x": { "a": 1 }
            },
            {
                "type": "Feature",
                "geometry": { "type": "Point", "coordinates": [1,1] },
                "properties": {},
                "y": [ {"deep":[1,2]}, {"more": null} ]
            }
        ]
    })";

    auto result = IO::Parse_Geojson_String(json);
    ASSERT_TRUE(result.Has_Value()) << "Parser failed on featurecollection with foreign keys";
    ASSERT_TRUE(result.Value().Is_Feature_Collection());
    const auto& fc = result.Value().Get_Feature_Collection();
    ASSERT_EQ(fc.features.size(), 2u);
    EXPECT_TRUE(fc.features[0].geometry.Is_Point());
    EXPECT_TRUE(fc.features[1].geometry.Is_Point());
    AssertPositionEquals(fc.features[0].geometry.Get_Point().position, ExpectedPos{0,0,std::nullopt});
    AssertPositionEquals(fc.features[1].geometry.Get_Point().position, ExpectedPos{1,1,std::nullopt});
}

TEST_F(Foreign_Test, Deeply_Nested_Foreign_Structures) {
    // construct a deep nesting (object->array->object->array...) as string
    std::string deep = R"("deep")";
    for (int i = 0; i < 20; ++i) {
        if (i % 2 == 0) deep = std::string("[") + deep + "]";
        else             deep = std::string("{\"k\":") + deep + "}";
    }

    std::string json = std::string(R"({
        "type": "Point",
        "coordinates": [0,0],
        "foreign_deep": )") + deep + "\n}";

    auto result = IO::Parse_Geojson_String(json);
    ASSERT_TRUE(result.Has_Value()) << "Parser failed on deeply nested foreign structures";
    ASSERT_TRUE(result.Value().Is_Geometry());
    AssertPositionEquals(result.Value().Get_Geometry().Get_Point().position, ExpectedPos{0,0,std::nullopt});
}

TEST_F(Foreign_Test, Duplicate_Foreign_Keys_At_Different_Levels) {
    std::string json = R"({
        "type": "Feature",
        "id": "dup",
        "geometry": { "type": "Point", "coordinates": [9,9], "x": 1 },
        "properties": { "core": "ok" },
        "x": { "root": true }
    })";

    auto result = IO::Parse_Geojson_String(json);
    ASSERT_TRUE(result.Has_Value()) << "Parser failed on duplicate foreign keys";
    ASSERT_TRUE(result.Value().Is_Feature());
    const auto& f = result.Value().Get_Feature();
    EXPECT_EQ(*f.id, "dup");
    AssertPositionEquals(f.geometry.Get_Point().position, ExpectedPos{9.0, 9.0, std::nullopt});
    // properties unaffected:
    ASSERT_TRUE(f.properties.Is_Object());
    EXPECT_TRUE(f.properties.Get_Object().find("core") != f.properties.Get_Object().end());
}