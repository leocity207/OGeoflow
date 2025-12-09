#ifndef SRC_DCEL_TEST_REVERSE_EXEMPLE_H
#define SRC_DCEL_TEST_REVERSE_EXEMPLE_H

#include <string>
#include <array>
#include <vector>
#include "dcel/constant.h"


struct Reverse_Exemple
{
	static inline const std::string json = R"({
			"type": "FeatureCollection",
			"features": [
				{
					"type": "Feature",
					"geometry": { 
						"type": "Polygon",
						"coordinates": [[[0, 0], [1, 0], [1, 1], [0, 0]]] 
					},
					"properties": {}
				},
				{
					"type": "Feature",
					"geometry": { 
						"type": "Polygon",
						"coordinates": [[[2, 0], [2, 1], [3, 0], [2, 0]]] 
					},
					"properties": {}
				},
				{
					"type": "Feature",
					"geometry": { 
						"type": "Polygon",
						"coordinates": [[[1, 1], [2, 1], [2, 0], [1, 0], [1, 1]]] 
					},
					"properties": {}
				}
			]
	})";

	static inline const std::string expected_write = R"({"type":"FeatureCollection","features":[{"type":"Feature","geometry":{"type":"Polygon","coordinates":[[[0.0,0.0],[1.0,0.0],[1.0,1.0],[0.0,0.0]]]},"properties":{}},{"type":"Feature","geometry":{"type":"Polygon","coordinates":[[[3.0,0.0],[2.0,1.0],[2.0,0.0],[3.0,0.0]]]},"properties":{}},{"type":"Feature","geometry":{"type":"Polygon","coordinates":[[[1.0,0.0],[2.0,0.0],[2.0,1.0],[1.0,1.0],[1.0,0.0]]]},"properties":{}}]})";
	
	static inline const auto expected_coords = std::to_array<std::pair<double, double>>({
		{0,0},
		{1,0},
		{1,1},
		{2,0},
		{2,1},
		{3,0}
	});

	static inline const auto expected_out_edges = std::vector<std::vector<int>>{
		{0, 5},
		{12, 2, 1},
		{4, 3, 15},
		{10, 9, 13},
		{14, 8, 7},
		{6, 11}
	};

	static inline const auto expected_origins = std::vector<int>{
		0,
		1,
		1,
		2,
		2,
		0,
		5,
		4,
		4,
		3,
		3,
		5,
		1,
		3,
		4,
		2,
	};

	static inline const auto expected_twins = std::vector<int>{
		1,0,
		3,2,
		5,4,
		7,6,
		9,8,
		11,10,
		13,12,
		15,14
	};

	static inline const auto expected_prevs = std::vector<int>{
		4,
		13,
		0,
		14,
		2,
		1,
		10,
		15,
		6,
		12,
		8,
		7,
		3,
		11,
		9,
		5,
	};

	static inline const auto expected_nexts = std::vector<int>{
		2,
		5,
		4,
		12,
		0,
		15,
		8,
		11,
		10,
		14,
		6,
		13,
		9,
		1,
		3,
		7,
	};

	static inline const auto expected_faces = std::vector<int>{
		0,
		3,
		0,
		2,
		0,
		3,
		1,
		3,
		1,
		2,
		1,
		3,
		2,
		3,
		2,
		3
	};

	static inline const auto expected_edges = std::to_array<size_t>({
		0,
		6,
		12,
		1,
	});

	static inline const auto expected_features = std::vector<size_t>{
		0,
		1,
		2,
		O::DCEL::NO_IDX
	};

	static inline const auto expected_outer_faces= std::vector<size_t>{
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX,
		O::DCEL::NO_IDX
	};
};

#endif //SRC_DCEL_TEST_REVERSE_EXEMPLE_H