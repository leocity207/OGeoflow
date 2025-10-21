#include "level3_coordinate_test.h"

#include <ranges>
#include "include/io/parser.h"
#include "parser_helper_test.h"

TEST_F(Level3_Coordinate_Test, Polygon_Without_Altitude) {
    std::string json = R"({
        "type": "Polygon",
        "coordinates": [
            [
                [100.0, 9.02],
                [101.0, 8.0],
                [102.0, 7.0],
                [103.0, 8.0],
                [100.0, 9.02]
            ]
        ]
    })";

    auto geometry = Parse_To_Geometry(json);
    ASSERT_TRUE(geometry.Is_Polygon());
    auto& polygon = geometry.Get_Polygon();

    ASSERT_EQ(polygon.rings.size(), 1u);
    std::vector<ExpectedPos> expected = {
        {100.0, 9.02, std::nullopt},
        {101.0, 8.0,  std::nullopt},
        {102.0, 7.0,  std::nullopt},
        {103.0, 8.0,  std::nullopt},
        {100.0, 9.02, std::nullopt}
    };
    AssertPositionsEquals(polygon.rings[0], expected);
}

TEST_F(Level3_Coordinate_Test, Polygon_With_Mixed_Altitude) {
    std::string json = R"({
        "type": "Polygon",
        "coordinates": [
            [
                [100.0, 9.02, 1000],
                [101.0, 8.0],
                [102.0, 7.0, -1000],
                [103.0, 8.0, -0.5],
                [100.0, 9.02, 1000]
            ]
        ]
    })";

    auto geometry = Parse_To_Geometry(json);
    ASSERT_TRUE(geometry.Is_Polygon());
    auto& polygon = geometry.Get_Polygon();

    ASSERT_EQ(polygon.rings.size(), 1u);
    std::vector<ExpectedPos> expected = {
        {100.0, 9.02, 1000.0},
        {101.0, 8.0,  std::nullopt},
        {102.0, 7.0, -1000.0},
        {103.0, 8.0, -0.5},
        {100.0, 9.02, 1000.0}
    };
    AssertPositionsEquals(polygon.rings[0], expected);
}


TEST_F(Level3_Coordinate_Test, Polygon_With_Multiple_Rings) {
    std::string json = R"({
        "type": "Polygon",
        "coordinates": [
            [
                [100.0, 9.02],
                [101.0, 8.0],
                [102.0, 7.0],
                [100.0, 9.02]
            ],
            [
                [200.0, 19.02],
                [201.0, 18.0],
                [202.0, 17.0],
                [200.0, 19.02]
            ],
            [
                [0.0, 0.0],
                [1.0, 0.0],
                [1.0, 1.0],
                [0.0, 0.0]
            ]
        ]
    })";

    auto geometry = Parse_To_Geometry(json);
    ASSERT_TRUE(geometry.Is_Polygon());
    auto& polygon = geometry.Get_Polygon();

    ASSERT_EQ(polygon.rings.size(), 3u);

    std::vector<ExpectedPos> ring0 = {
        {100.0, 9.02, std::nullopt},
        {101.0, 8.0,  std::nullopt},
        {102.0, 7.0,  std::nullopt},
        {100.0, 9.02, std::nullopt}
    };
    std::vector<ExpectedPos> ring1 = {
        {200.0, 19.02, std::nullopt},
        {201.0, 18.0,  std::nullopt},
        {202.0, 17.0,  std::nullopt},
        {200.0, 19.02, std::nullopt}
    };
    std::vector<ExpectedPos> ring2 = {
        {0.0, 0.0, std::nullopt},
        {1.0, 0.0, std::nullopt},
        {1.0, 1.0, std::nullopt},
        {0.0, 0.0, std::nullopt}
    };

    AssertPositionsEquals(polygon.rings[0], ring0);
    AssertPositionsEquals(polygon.rings[1], ring1);
    AssertPositionsEquals(polygon.rings[2], ring2);
}

TEST_F(Level3_Coordinate_Test, Multi_Line_String_Without_Altitude) {
    std::string json = R"({
        "type": "MultiLineString",
        "coordinates": [
            [
                [100.0, 9.02],
                [101.0, 8.0],
                [102.0, 7.0]
            ]
        ]
    })";

    auto geometry = Parse_To_Geometry(json);
    ASSERT_TRUE(geometry.Is_Multi_Line_String());
    auto& mls = geometry.Get_Multi_Line_String();

    ASSERT_EQ(mls.line_strings.size(), 1u);
    std::vector<ExpectedPos> expected = {
        {100.0, 9.02, std::nullopt},
        {101.0, 8.0,  std::nullopt},
        {102.0, 7.0,  std::nullopt}
    };
    AssertPositionsEquals(mls.line_strings[0].positions, expected);
}

TEST_F(Level3_Coordinate_Test, Multi_Line_String_With_Multiple_Lines) {
    std::string json = R"({
        "type": "MultiLineString",
        "coordinates": [
            [
                [100.0, 9.02],
                [101.0, 8.0]
            ],
            [
                [1.0, 2.0,  10],
                [3.0, 4.0, -5],
                [5.0, 6.0]
            ],
            [
                [0.0, 0.0],
                [0.1, 0.1]
            ]
        ]
    })";

    auto geometry = Parse_To_Geometry(json);
    ASSERT_TRUE(geometry.Is_Multi_Line_String());
    auto& mls = geometry.Get_Multi_Line_String();

    ASSERT_EQ(mls.line_strings.size(), 3u);

    std::vector<ExpectedPos> l0 = {
        {100.0, 9.02, std::nullopt},
        {101.0, 8.0,  std::nullopt}
    };
    std::vector<ExpectedPos> l1 = {
        {1.0, 2.0,  10.0},
        {3.0, 4.0,  -5.0},
        {5.0, 6.0,  std::nullopt}
    };
    std::vector<ExpectedPos> l2 = {
        {0.0, 0.0, std::nullopt},
        {0.1, 0.1, std::nullopt}
    };

    AssertPositionsEquals(mls.line_strings[0].positions, l0);
    AssertPositionsEquals(mls.line_strings[1].positions, l1);
    AssertPositionsEquals(mls.line_strings[2].positions, l2);
}

// Polygon : anneau trop petit (moins de 4 positions) -> erreur
TEST_F(Level3_Coordinate_Test, Polygon_Not_Enough_Points) {
    std::string json = R"({
        "type": "Polygon",
        "coordinates": [
            [
                [100.0, 9.02],
                [101.0, 8.0],
                [102.0, 7.0]
            ]
        ]
    })";

    // adjust the expected error name if your IO::Error enum differs
    Expect_Parse_Error(json, IO::Error::Type::NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON);
}

// Polygon : anneau non fermé (dernier point != premier) -> erreur
TEST_F(Level3_Coordinate_Test, Polygon_Not_Closed) {
    std::string json = R"({
        "type": "Polygon",
        "coordinates": [
            [
                [100.0, 9.02],
                [101.0, 8.0],
                [102.0, 7.0],
                [103.0, 8.0] 
            ]
        ]
    })";

    Expect_Parse_Error(json, IO::Error::Type::POLYGON_NEED_TO_BE_CLOSED);
}

TEST_F(Level3_Coordinate_Test, With_Mismatch_Level) {
    std::string json = R"({
        "type": "MultiLineString",
        "coordinates": [
            [
                [100.0, 9.02],
                [101.0, 8.0]
            ],
            42.5,
            [
                [0.0, 0.0]
            ]
        ]
    })";

    Expect_Parse_Error(json, IO::Error::Type::INCONSCISTENT_COORDINATE_LEVEL);
}
