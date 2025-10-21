#ifndef SRC_IO_TEST_FOREIGN_TEST_H
#define SRC_IO_TEST_FOREIGN_TEST_H

#include <gtest/gtest.h>

class Foreign_Test : public ::testing::Test {};

//////////////////////////////////////////////
/// Nominal tests:
/// 	- Root_Foreign_Primitives_And_Null_Do_Not_Break_Parse
/// 	- Root_Foreign_Object_And_Array_Nested_Do_Not_Affect_Main
/// 	- Feature_Level_Foreign_Key_Mixed_Types
/// 	- Geometry_Level_Foreign_Key_Does_Not_Break_Geometry
/// 	- Root_Foreign_Array_Complex_Elements
/// 	- Feature_Collection_Multiple_Features_With_Foreign_Keys
/// 	- Deeply_Nested_Foreign_Structures
/// 	- Duplicate_Foreign_Keys_At_Different_Levels
//////////////////////////////////////////////

#endif //SRC_IO_TEST_FOREIGN_TEST_H