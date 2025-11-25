#ifndef GEOJSON_POSITION_H
#define GEOJSON_POSITION_H

#include <optional>

namespace O::GeoJSON
{

	struct Position
	{
		double longitude = 0;
		double latitude = 0;
		std::optional<double> altitude;
	};
}

#endif //GEOJSON_POSITION_H