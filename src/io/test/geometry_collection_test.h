#ifndef SRC_IO_TEST_GEOMETRY_COLLECTION_TEST_H
#define SRC_IO_TEST_GEOMETRY_COLLECTION_TEST_H

#include <gtest/gtest.h>

class Geometry_Collection_Test : public ::testing::Test {};

//////////////////////////////////////////////
/// Nominal tests:
/// 	- Single_Point_In_Collection
/// 	- Mixed_Geometries_In_Collection
/// 	- Empty_Geometry_Collection
/// 	- Geometry_Collection_With_BBox
///     - Nested_Geometry_Collection
/// Error tests:
/// 	- Unknown_Geometry_Type_Inside_Collection_Should_Error
//////////////////////////////////////////////

#endif //SRC_IO_TEST_GEOMETRY_COLLECTION_TEST_H