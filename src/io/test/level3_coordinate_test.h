#ifndef SRC_IO_TEST_LEVEL_3_COORDINATE_TEST_H
#define SRC_IO_TEST_LEVEL_3_COORDINATE_TEST_H

#include <gtest/gtest.h>

class Level3_Coordinate_Test : public ::testing::Test {};

//////////////////////////////////////////////
/// Nominal tests:
/// 	- Polygon_Without_Altitude
/// 	- Polygon_With_Mixed_Altitude
/// 	- Polygon_With_Multiple_Rings
/// 	- Multi_Line_String_Without_Altitude
/// 	- Multi_Line_String_With_Multiple_Lines
/// Error tests:
/// 	- Polygon_Not_Enough_Points
/// 	- Polygon_Not_Closed
/// 	- Polygon_No_Rings
/// 	- With_Mismatch_Level
//////////////////////////////////////////////

#endif //SRC_IO_TEST_LEVEL_3_COORDINATE_TEST_H