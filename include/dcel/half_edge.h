#ifndef DCEL_HALF_EDGE_H
#define DCEL_HALF_EDGE_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <limits>

#include "constant.h"

namespace O::DCEL
{
	struct Half_Edge
	{
		size_t origin = NO_IDX; // index of origin vertex
		size_t twin = NO_IDX; // index of twin halfedge (reverse direction)
		size_t next = NO_IDX; // index of next halfedge around face
		size_t prev = NO_IDX; // index of previous halfedge around face
		size_t face = NO_IDX; // index of incident face (left side)
	};
} // namespace DCEL

#endif // DCEL_HALF_EDGE_H
