#ifndef SRC_DCEL_TEST_MULTI_POLYGON_EXEMPLE_H
#define SRC_DCEL_TEST_MULTI_POLYGON_EXEMPLE_H

#include <string>
#include <array>
#include <vector>
#include "dcel/constant.h"

struct Multi_Polygon_Exemple
{
	static inline const std::string json = R"({
		"type": "FeatureCollection",
		"features": [
			{
				"type": "Feature",
				"geometry": { 
					"type": "MultiPolygon",
					"coordinates": [
						[
							[[0, 0], [5, 0], [5, 5], [0, 5], [0,0]],
							[[1, 1], [1, 4], [2, 4], [2, 1], [1,1]],
							[[3, 1], [3, 4], [4, 4], [4, 1], [3,1]]
						],
						[
							[[5, 0], [5, 5], [6, 5], [6, 0], [5, 0]]
						]
					] 
				},
				"properties": {}
			}
		]
	})";

	static inline const auto expected_coords = std::to_array<std::pair<double, double>>({
		{0,0},
		{5,0},
		{5,5},
		{0,5},
		{1,1},
		{1,4},
		{2,4},
		{2,1},
		{3,1},
		{3,4},
		{4,4},
		{4,1},
		{6,5},
		{6,0}
	});

	static inline const auto expected_out_edges = std::vector<std::vector<int>>{
		{0,7},
		{28,2,1},
		{4,3,27},
		{6,5},
		{14,13},
		{12,11},
		{10,9},
		{8,15},
		{22,21},
		{20,19},
		{18,17},
		{16,23},
		{26,25},
		{24,29}
	};

	static inline const auto expected_origins = std::vector<int>{
		0,
		1,
		1,
		2,
		2,
		3,
		3,
		0,
		7,
		6,
		6,
		5,
		5,
		4,
		4,
		7,
		11,
		10,
		10,
		9,
		9,
		8,
		8,
		11,
		13,
		12,
		12,
		2,
		1,
		13
	};

	static inline const auto expected_twins = std::vector<int>{
		1,0,
		3,2,
		5,4,
		7,6,
		9,8,
		11,10,
		13,12,
		15,14,
		17,16,
		19,18,
		21,20,
		23,22,
		25,24,
		27,26,
		29,28
	};

	static inline const auto expected_prevs = std::vector<int>{
		6,
		29,
		0,
		26,
		2,
		7,
		4,
		1,
		14,
		11,
		8,
		13,
		10,
		15,
		12,
		9,
		22,
		19,
		16,
		21,
		18,
		23,
		20,
		17,
		28,
		27,
		24,
		5,
		3,
		25
	};

	static inline const auto expected_nexts = std::vector<int>{
		2,
		7,
		4,
		28,
		6,
		27,
		0,
		5,
		10,
		15,
		12,
		9,
		14,
		11,
		8,
		13,
		18,
		23,
		20,
		17,
		22,
		19,
		16,
		21,
		26,
		29,
		3,
		25,
		24,
		1
	};

	static inline const auto expected_faces = std::vector<int>{
		0,
		4,
		0,
		0,
		0,
		4,
		0,
		4,
		5,
		0,
		5,
		0,
		5,
		0,
		5,
		0,
		6,
		0,
		6,
		0,
		6,
		0,
		6,
		0,
		0,
		4,
		0,
		4,
		0,
		4
	};

	static inline const auto expected_edges = std::to_array<size_t>({
		0,
		9,
		17,
		24,
		1,
		8,
		16
	});

	static inline const auto expected_features = std::vector<size_t>{
		0,
		0,
		0,
		0,
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX
	};

	static inline const auto expected_outer_faces= std::vector<size_t>{
		O::DCEL::NO_IDX,
		0,
		0,
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX
	};

};

#endif //SRC_DCEL_TEST_MULTI_POLYGON_EXEMPLE_H