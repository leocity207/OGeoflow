#ifndef SRC_IO_TEST_LEVEL_4_COORDINATE_TEST_H
#define SRC_IO_TEST_LEVEL_4_COORDINATE_TEST_H

#include <gtest/gtest.h>

class Level4_Coordinate_Test : public ::testing::Test {};

//////////////////////////////////////////////
/// Nominal tests:
/// 	- Multi_Polygon_Single_Polygon_No_Altitude
/// 	- Multi_Polygon_Multiple_Simple_Polygons
/// 	- Multi_Polygon_Complex_Polygons
/// 	- Multi_Polygon_Mixed_Altitude
/// Error tests:
/// 	- Multi_Polygon_Ring_Too_Small
/// 	- Multi_Polygon_Ring_Not_Closed
/// 	- Multi_Polygon_Mismatch_Level
//////////////////////////////////////////////

#endif //SRC_IO_TEST_LEVEL_4_COORDINATE_TEST_H