#ifndef DCEL_FACE_H
#define DCEL_FACE_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <limits>

#include "constant.h"

namespace O::DCEL
{
	/**
	 * @brief A face is way to create half_edge cycle inside the DCEL gaph.
	 *        In our case face are tightly linked to Polygone of the GeoJSON
	 * 
	 */
	struct Face
	{
		size_t edge = NO_IDX;               ///< a representative halfedge belonging to this face
		size_t associated_feature = NO_IDX; ///< indices of GeoJSON features that reference this face
		size_t outer_face = NO_IDX;
	};

} // namespace DCEL

#endif // DCEL_FACE_H
