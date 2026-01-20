#ifndef SRC_DCEL_SIMPLE_EXEMPLE_H
#define SRC_DCEL_SIMPLE_EXEMPLE_H

#include <string>
#include <array>
#include <vector>

struct Simple_Exemple
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
						"coordinates": [[[1, 0], [1, 1], [2, 0], [1, 0]]] 
					},
					"properties": {}
				},
				{
					"type": "Feature",
					"geometry": { 
						"type": "Polygon",
						"coordinates": [[[0, 0], [1, 0], [2, 0], [3, -1], [2, -2], [0, -2], [0, 0]]] 
					},
					"properties": {}
				}
			]
	})";

	static inline const std::string expected_write = R"({"type":"FeatureCollection","features":[{"type":"Feature","geometry":{"type":"Polygon","coordinates":[[[0.0,0.0],[1.0,0.0],[1.0,1.0],[0.0,0.0]]]},"properties":{}},{"type":"Feature","geometry":{"type":"Polygon","coordinates":[[[2.0,0.0],[1.0,1.0],[1.0,0.0],[2.0,0.0]]]},"properties":{}},{"type":"Feature","geometry":{"type":"Polygon","coordinates":[[[0.0,-2.0],[2.0,-2.0],[3.0,-1.0],[2.0,0.0],[1.0,0.0],[0.0,0.0],[0.0,-2.0]]]},"properties":{}}]})";

	static inline const auto expected_coords = std::to_array<std::pair<double, double>>({
		{0,0},
		{1,0},
		{1,1},
		{2,0},
		{3,-1},
		{2,-2},
		{0,-2}
	});

	static inline const auto expected_out_edges = std::vector<std::vector<int>>{
		{16,0, 5},
		{8, 2, 1},
		{4, 3, 7},
		{6, 9, 15},
		{14,13},
		{12,11},
		{10,17}
	};

	static inline const auto expected_tails = std::vector<int>{
		0,
		1,
		1,
		2,
		2,
		0,
		3,
		2,
		1,
		3,
		6,
		5,
		5,
		4,
		4,
		3,
		0,
		6
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
		17,16
	};

	static inline const auto expected_prevs = std::vector<int>{
		4,
		9,
		0,
		6,
		2,
		17,
		8,
		5,
		3,
		14,
		16,
		13,
		10,
		15,
		12,
		7,
		1,
		11,
	};

	static inline const auto expected_nexts = std::vector<int>{
		2,
		16,
		4,
		8,
		0,
		7,
		3,
		15,
		6,
		1,
		12,
		17,
		14,
		11,
		9,
		13,
		10,
		5
	};

	static inline const auto expected_faces = std::vector<int>{
		0,
		2,
		0,
		1,
		0,
		3,
		1,
		3,
		1,
		2,
		2,
		3,
		2,
		3,
		2,
		3,
		2,
		3
	};

	static inline const auto expected_edges = std::to_array<size_t>({
		0,
		6,
		10,
		5,
	});
};

#endif //SRC_DCEL_SIMPLE_EXEMPLE_H