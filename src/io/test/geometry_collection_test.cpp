#include "geometry_collection_test.h"

#include "include/io/parser.h"
#include "parser_helper_test.h"

TEST_F(Geometry_Collection_Test, Single_Point_In_Collection) {
    std::string json = R"({
        "type": "GeometryCollection",
        "geometries": [
            { "type": "Point", "coordinates": [10.0, 20.0] }
        ]
    })";

    auto geometry = Parse_To_Geometry(json);
    ASSERT_TRUE(geometry.Is_Geometry_Collection());
    const auto& gc = geometry.Get_Geometry_Collection();
    ASSERT_EQ(gc.geometries.size(), 1u);

    const auto& g0 = *gc.geometries[0];
    EXPECT_TRUE(g0.Is_Point());
    const auto& pt = g0.Get_Point();
    AssertPositionEquals(pt.position, ExpectedPos{10.0, 20.0, std::nullopt});
}

TEST_F(Geometry_Collection_Test, Mixed_Geometries_In_Collection) {
    std::string json = R"({
        "type": "GeometryCollection",
        "geometries": [
            { "type": "Point", "coordinates": [1.0, 2.0] },
            { "type": "LineString", "coordinates": [
                [3.0, 4.0],
                [5.0, 6.0],
                [7.5, 8.5]
            ] }
        ]
    })";

    auto geometry = Parse_To_Geometry(json);
    ASSERT_TRUE(geometry.Is_Geometry_Collection());
    const auto& gc = geometry.Get_Geometry_Collection();
    ASSERT_EQ(gc.geometries.size(), 2u);

    const auto& g0 = *gc.geometries[0];
    ASSERT_TRUE(g0.Is_Point());
    AssertPositionEquals(g0.Get_Point().position, ExpectedPos{1.0, 2.0, std::nullopt});

    const auto& g1 = *gc.geometries[1];
    ASSERT_TRUE(g1.Is_Line_String());
    const auto& line = g1.Get_Line_String();
    std::vector<ExpectedPos> expected = {
        {3.0, 4.0, std::nullopt},
        {5.0, 6.0, std::nullopt},
        {7.5, 8.5, std::nullopt}
    };
    AssertPositionsEquals(line.positions, expected);
}

TEST_F(Geometry_Collection_Test, Empty_Geometry_Collection) {
    std::string json = R"({
        "type": "GeometryCollection",
        "geometries": []
    })";

    auto geometry = Parse_To_Geometry(json);
    ASSERT_TRUE(geometry.Is_Geometry_Collection());
    const auto& gc = geometry.Get_Geometry_Collection();
    EXPECT_EQ(gc.geometries.size(), 0u);
}

TEST_F(Geometry_Collection_Test, Geometry_Collection_With_BBox) {
    std::string json = R"({
        "type": "GeometryCollection",
        "bbox": [0.0, 0.0, 10.0, 10.0],
        "geometries": [
            { "type": "Point", "coordinates": [2.0, 3.0] }
        ]
    })";

    auto geometry = Parse_To_Geometry(json);
    ASSERT_TRUE(geometry.Is_Geometry_Collection());

    ASSERT_TRUE(geometry.bbox.has_value());

    ASSERT_FALSE(geometry.bbox->Has_Altitude());
    auto arr = geometry.bbox->Get();
    EXPECT_DOUBLE_EQ(arr[0], 0.0);
    EXPECT_DOUBLE_EQ(arr[1], 0.0);
    EXPECT_DOUBLE_EQ(arr[2], 10.0);
    EXPECT_DOUBLE_EQ(arr[3], 10.0);

    const auto& gc = geometry.Get_Geometry_Collection();
    ASSERT_EQ(gc.geometries.size(), 1u);
    ASSERT_TRUE((*gc.geometries[0]).Is_Point());
    AssertPositionEquals((*gc.geometries[0]).Get_Point().position, ExpectedPos{2.0, 3.0, std::nullopt});
}

TEST_F(Geometry_Collection_Test, Nested_Geometry_Collection) {
    std::string json = R"({
        "type": "GeometryCollection",
        "geometries": [
            {
                "type": "GeometryCollection",
                "geometries": [
                    { "type": "Point", "coordinates": [11.0, 12.0] }
                ]
            },
            { "type": "Point", "coordinates": [1.0, 1.0] }
        ]
    })";

    auto geometry = Parse_To_Geometry(json);
    ASSERT_TRUE(geometry.Is_Geometry_Collection());
    const auto& outer_gc = geometry.Get_Geometry_Collection();
    ASSERT_EQ(outer_gc.geometries.size(), 2u);

    const auto& nested_ptr = outer_gc.geometries[0];
    ASSERT_NE(nested_ptr, nullptr);
    const auto& nested_geom = *nested_ptr;
    ASSERT_TRUE(nested_geom.Is_Geometry_Collection());
    const auto& inner_gc = nested_geom.Get_Geometry_Collection();
    ASSERT_EQ(inner_gc.geometries.size(), 1u);
    ASSERT_TRUE((*inner_gc.geometries[0]).Is_Point());
    AssertPositionEquals((*inner_gc.geometries[0]).Get_Point().position, ExpectedPos{11.0, 12.0, std::nullopt});

    ASSERT_TRUE((*outer_gc.geometries[1]).Is_Point());
    AssertPositionEquals((*outer_gc.geometries[1]).Get_Point().position, ExpectedPos{1.0, 1.0, std::nullopt});
}

TEST_F(Geometry_Collection_Test, Unknown_Geometry_Type_Inside_Collection_Should_Error) {
    std::string json = R"({
        "type": "GeometryCollection",
        "geometries": [
            { "type": "Point", "coordinates": [0.0, 0.0] },
            { "type": "NotARealGeometryType", "coordinates": [1.0, 2.0] }
        ]
    })";

    Expect_Parse_Error(json, GeoJSON::IO::Error::Type::UNKNOWN_TYPE);
}