
#include <gtest/gtest.h>
#include "parser_test.h"

class Point_Geometry_Test : public GeoJSON_Parser_Test, public ::testing::WithParamInterface<std::tuple<std::string, double, double, double, bool>> {};