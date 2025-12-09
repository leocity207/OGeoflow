#ifndef SRC_DCEL_TEST_HOLE_EXEMPLE_H
#define SRC_DCEL_TEST_HOLE_EXEMPLE_H

#include <string>
#include <array>
#include <vector>
#include "dcel/constant.h"

struct Hole_Exemple
{
	static inline const std::string json = R"({
			"type": "FeatureCollection",
			"features": [
				{
					"type": "Feature",
					"geometry": { 
						"type": "Polygon",
						"coordinates": [					
								[[0, 0], [5, 0], [5, 5], [0, 5], [0,0]],
								[[1, 1], [1, 4], [2, 4], [2, 1], [1,1]],
								[[3, 1], [3, 4], [4, 4], [4, 1], [3,1]]
						] 
					},
					"properties": {}
				},
				{
					"type": "Feature",
					"geometry": { 
						"type": "Polygon",
						"coordinates": [[[1, 1], [1, 4], [2, 4], [2, 1], [1,1]]] 
					},
					"properties": {}
				}
			]
	})";

	static inline const std::string expected_write = R"({"type":"FeatureCollection","features":[{"type":"Feature","geometry":{"type":"Polygon","coordinates":[[[0.0,0.0],[5.0,0.0],[5.0,5.0],[0.0,5.0],[0.0,0.0]],[[2.0,4.0],[2.0,1.0],[1.0,1.0],[1.0,4.0],[2.0,4.0]],[[4.0,4.0],[4.0,1.0],[3.0,1.0],[3.0,4.0],[4.0,4.0]]]},"properties":{}},{"type":"Feature","geometry":{"type":"Polygon","coordinates":[[[2.0,1.0],[2.0,4.0],[1.0,4.0],[1.0,1.0],[2.0,1.0]]]},"properties":{}}]})";

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
	});

	static inline const auto expected_out_edges = std::vector<std::vector<int>>{
		{0, 7},
		{2, 1},
		{4, 3},
		{6, 5},
		{14, 13},
		{12, 11},
		{10, 9},
		{8, 15},
		{22, 21},
		{20, 19},
		{18, 17},
		{16, 23}
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
		11
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
		23,22
	};

	static inline const auto expected_prevs = std::vector<int>{
		6,
		3,
		0,
		5,
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
		17
	};

	static inline const auto expected_nexts = std::vector<int>{
		2,
		7,
		4,
		1,
		6,
		3,
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
		21
	};

	static inline const auto expected_faces = std::vector<int>{
		0,
		4,
		0,
		4,
		0,
		4,
		0,
		4,
		1,
		0,
		1,
		0,
		1,
		0,
		1,
		0,
		5,
		0,
		5,
		0,
		5,
		0,
		5,
		0
	};

	static inline const auto expected_edges = std::to_array<size_t>({
		0,
		9,
		17,
		8,
		1,
		16
	});

	static inline const auto expected_features = std::vector<size_t>{
		0,
		0,
		0,
		1,
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX
	};

	static inline const auto expected_outer_faces= std::vector<size_t>{
		O::DCEL::NO_IDX,
		0,
		0,
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX
	};
};

#endif //SRC_DCEL_TEST_HOLE_EXEMPLE_H