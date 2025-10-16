#ifndef GEOJSON_POSITION_H
#define GEOJSON_POSITION_H

#include <optional>

namespace GeoJSON
{

	struct Position {
		double longitude;
		double latitude;
		std::optional<double> altitude;

		Position();
		Position(double lon, double lat);
		Position(double lon, double lat, double alt);
	};
}

#endif //GEOJSON_POSITION_H