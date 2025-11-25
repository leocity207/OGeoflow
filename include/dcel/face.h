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
	struct Face
	{
		size_t edge = NO_IDX; // a representative halfedge belonging to this face
		size_t associated_feature = NO_IDX; // indices of features that reference this face
		std::vector<size_t> inner_edges; // representative edges for holes (if any)

		size_t outer_face = NO_IDX;
	};

} // namespace DCEL

#endif // DCEL_FACE_H
