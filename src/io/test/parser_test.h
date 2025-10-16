#include <gtest/gtest.h>
#include <rapidjson/document.h>

#include "include/io/parser.h"


class GeoJSON_Parser_Test : public ::testing::Test {
protected:
	void SetUp() override {
		// Common setup for all tests
	}

	void TearDown() override {
		// Clean up after each test
	}

	bool Is_Point_Equal(const GeoJSON::Position& p1, const GeoJSON::Position& p2, double tolerance = 1e-10) {
		if (p1.altitude.has_value() && p2.altitude.has_value())
			return std::abs(p1.longitude - p2.longitude) < tolerance &&
				std::abs(p1.latitude - p2.latitude) < tolerance &&
				std::abs(*p1.altitude - *p2.altitude) < tolerance;
		else if (!p1.altitude.has_value() && !p2.altitude.has_value())
			return std::abs(p1.longitude - p2.longitude) < tolerance &&
				std::abs(p1.latitude - p2.latitude) < tolerance;
		else
			return false;
	}
};