#ifndef GEOJSON_POSITION_H
#define GEOJSON_POSITION_H

#include <optional>

namespace O::GeoJSON
{
	/**
	 * @brief Represents a geographic position in GeoJSON.
	 *        A Position defines a single coordinate tuple consisting of longitude, latitude, and optionally altitude. According to the GeoJSON specification, the order of coordinates is `[longitude, latitude, (altitude)]`.
	 */
	struct Position
	{
		double longitude = 0;           ///< Longitude value, in decimal degrees (X axis).
		double latitude = 0;            ///< Latitude value, in decimal degrees (Y axis).
		std::optional<double> altitude; ///< Optional altitude (Z coordinate), in meters above mean sea level.
	};
}

#endif // GEOJSON_POSITION_H