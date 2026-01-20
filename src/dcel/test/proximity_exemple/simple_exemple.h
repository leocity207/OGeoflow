#ifndef SRC_DCEL_SIMPLE_PROXIMITY_EXEMPLE_H
#define SRC_DCEL_SIMPLE_PROXIMITY_EXEMPLE_H

#include <string>
#include <array>
#include <vector>

struct Simple_Proximity_Exemple
{
	static inline const std::string json = R"({
			"type": "FeatureCollection",
			"features": [
				{
					"type": "Feature",
					"geometry": { 
						"type": "Polygon",
						"coordinates": [[[0, 0], [1, 0], [1, 0.001], [1.001, 1], [1, 1], [0, 0]]] 
					},
					"properties": {}
				}
			]
	})";

	static inline const auto expected_coords = std::to_array<std::pair<double, double>>({
	{0,0},
	{1,0},
	{1.001,1}});
};

#endif //SRC_DCEL_SIMPLE_PROXIMITY_EXEMPLE_H