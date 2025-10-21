#ifndef SRC_IO_TEST_FEATURE_TEST_H
#define SRC_IO_TEST_FEATURE_TEST_H

#include <gtest/gtest.h>

class Feature_Test : public ::testing::Test {};

//////////////////////////////////////////////
/// Nominal tests:
/// 	- Feature_Collection_Features_Basic
/// 	- Feature_Id_As_Number_Should_Be_Stringified
/// 	- Feature_Missing_Geometry_Should_Fail
/// 	- Feature_Properties_Null_Accepted
/// Error tests:
/// 	- Feature_Geometry_Null_Behavior
/// 	- Feature_With_Wrong_Type_In_Features_Array
//////////////////////////////////////////////

#endif //SRC_IO_TEST_FEATURE_TEST_H