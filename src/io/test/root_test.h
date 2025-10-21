#ifndef SRC_IO_TEST_ROOT_TEST_H
#define SRC_IO_TEST_ROOT_TEST_H

#include <gtest/gtest.h>

class Root_Test : public ::testing::Test {};

//////////////////////////////////////////////
/// Nominal tests:
/// 	- Root_Is_Geometry_Point
/// 	- Root_Is_Feature_Basic
/// 	- Feature_With_Id_And_BBox
/// 	- Feature_Collection_With_BBox
/// Error tests:
/// 	- Root_With_Wrong_Type_String_Fails
//////////////////////////////////////////////

#endif //SRC_IO_TEST_ROOT_TEST_H