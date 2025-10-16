
#include <gtest/gtest.h>
#include "parser_test.h"

class Coordinate_Validation_Test : public GeoJSON_Parser_Test, public ::testing::WithParamInterface<std::tuple<double, double, bool>> {};