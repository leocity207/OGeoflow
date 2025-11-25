#ifndef DCEL_STORAGE_H
#define DCEL_STORAGE_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <limits>

#include "face.h"
#include "half_edge.h"
#include "vertex.h"

namespace O::DCEL
{
	struct Storage
	{
		std::unordered_map<uint64_t, size_t> vertex_lookup; // hash -> vertex index
		std::vector<Vertex> vertices;
		std::unordered_map<uint64_t, size_t> edge_lookup; // (origin<<32 | head) -> halfedge index
		std::vector<Half_Edge> halfedges;
		std::vector<Face> faces;

		std::unordered_map<size_t, std::vector<size_t>> feature_to_faces; // featureIdx -> list of face indices
	};
} // namespace O::DCEL

#endif // DCEL_STORAGE_H
