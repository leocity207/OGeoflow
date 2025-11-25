#ifndef GEOJSON_POSITION_H
#define GEOJSON_POSITION_H

#include <optional>

namespace O::GeoJSON
{
	/**
	 * @brief Represents a geographic position in GeoJSON.
	 *
	 * A Position defines a single coordinate tuple consisting of longitude, latitude, and optionally altitude. According to the GeoJSON specification, the order
	 * of coordinates is `[longitude, latitude, (altitude)]`.
	 */
	struct Position
	{
		/// Longitude value, in decimal degrees (X axis).
		double longitude = 0;

		/// Latitude value, in decimal degrees (Y axis).
		double latitude = 0;

		/// Optional altitude (Z coordinate), in meters above mean sea level.
		std::optional<double> altitude;
	};
}

#endif // GEOJSON_POSITION_H