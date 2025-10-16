
#include <gtest/gtest.h>
#include "parser_test.h"

class Line_String_Geometry_Test : public GeoJSON_Parser_Test, public ::testing::WithParamInterface<std::tuple<std::string, std::vector<std::tuple<double, double>>>> {};