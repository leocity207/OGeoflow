#ifndef GEOJSON_FEATURE_COLLECTION_H
#define GEOJSON_FEATURE_COLLECTION_H

#include <vector>
#include <optional>
#include <string>
#include "feature.h"
#include "include/geojson/bbox.h"

namespace O::GeoJSON
{
	/**
	 * @brief Represents a GeoJSON FeatureCollection object.
	 *
	 * A FeatureCollection contains an array of individual Feature objects,
	 * optionally including a shared bounding box and collection identifier.
	 *
	 * This is the top-level object in most GeoJSON files.
	 *
	 * @see RFC 7946 §3.3 — FeatureCollection Objects
	 *
	 * Example JSON:
	 * @code
	 * {
	 *   "type": "FeatureCollection",
	 *   "features": [
	 *     {
	 *       "type": "Feature",
	 *       "geometry": { "type": "Point", "coordinates": [102.0, 0.5] },
	 *       "properties": { "name": "Example Point" }
	 *     },
	 *     {
	 *       "type": "Feature",
	 *       "geometry": { "type": "LineString", "coordinates": [[102.0, 0.0], [103.0, 1.0]] },
	 *       "properties": { "name": "Example Line" }
	 *     }
	 *   ]
	 * }
	 * @endcode
	 */
	struct Feature_Collection
	{
		/// Collection of individual feature objects.
		std::vector<Feature> features;

		/// Optional bounding box for the entire collection.
		std::optional<Bbox> bbox;

		/// Optional unique identifier for the collection.
		std::optional<std::string> id;
	};
}

#endif // GEOJSON_FEATURE_COLLECTION_H