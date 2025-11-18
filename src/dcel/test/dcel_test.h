#ifndef SRC_DCEL_TEST_DECEL_TEST_H
#define SRC_DCEL_TEST_DECEL_TEST_H

#include <gtest/gtest.h>

class DCEL_Builder_Exporter : public ::testing::Test {};

//////////////////////////////////////////////
/// Nominal tests:
/// 	- Line_String_Without_Altitude
/// 	- Line_String_With_Altitude
/// 	- Multi_Point_Without_Altitude
/// 	- Multi_Point_Without_Altitude
/// Error tests:
/// 	- With_Too_Much_Coordinate_In_Point
/// 	- With_Not_Enough_Coordinate_In_Point
/// 	- With_Not_Enough_Point
/// 	- With_Mismatch_Level
//////////////////////////////////////////////

#endif //SRC_DCEL_TEST_DECEL_TEST_H