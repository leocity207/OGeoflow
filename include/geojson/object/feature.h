#ifndef GEOJSON_FEATURE_H
#define GEOJSON_FEATURE_H

#include <variant>
#include <optional>
#include <string>
#include "geometry.h"
#include "geojson/properties.h"
#include "geojson/bbox.h"

namespace O::GeoJSON
{
	struct Feature_Collection;

	/**
	 * @brief Represents a GeoJSON Feature object.
	 *
	 * A Feature combines a spatial geometry with associated attribute data (properties) and optional metadata (identifier, bounding box).
	 * Features are the most common container for representing spatial entities, such as roads, buildings, or points of interest.
	 *
	 * @see RFC 7946 §3.2 — Feature Objects
	 *
	 * Example JSON:
	 * @code
	 * {
	 *   "type": "Feature",
	 *   "geometry": {
	 *       "type": "Point",
	 *       "coordinates": [102.0, 0.5]
	 *   },
	 *   "properties": {
	 *       "name": "Sample Point"
	 *   },
	 *   "id": "point_001"
	 * }
	 * @endcode
	 */
	struct Feature
	{
		/// Optional geometry object. May be null if the feature has no spatial representation.
		std::optional<Geometry> geometry;

		/// The feature's set of properties or attributes.
		Property properties;

		/// Optional unique identifier of the feature.
		std::optional<std::string> id;

		/// Optional bounding box for this feature.
		std::optional<Bbox> bbox;
	};
}

#endif // GEOJSON_FEATURE_H