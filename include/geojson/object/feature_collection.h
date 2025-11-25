#ifndef GEOJSON_FEATURE_COLLECTION_H
#define GEOJSON_FEATURE_COLLECTION_H

#include <vector>
#include "feature.h"

namespace O::GeoJSON
{
	struct Feature_Collection
	{
		std::vector<Feature> features;
		std::optional<Bbox> bbox;
		std::optional<std::string> id;
	};
}

#endif //GEOJSON_FEATURE_COLLECTION_H