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
	/**
	 * @brief Storage is structure that hold all the DCEL Information (``Vertex``, ``Half_Edge``, ``Face``).
	 *        The class also contain look up map for fast acess to linked feature.
	 *        We also embed a look up table that link face to their feature inside the GeoJSON it can be linked to the ``Feature_Info`` class
	 */
	struct Storage
	{
		std::vector<Vertex> vertices;      ///< List of Vertex in the DCEL
		std::vector<Half_Edge> half_edges; ///< List of Half_Edges in the DCEL
		std::vector<Face> faces;           ///< List of Faces in the DCEL

		std::unordered_map<uint64_t, size_t> vertex_lookup;               ///< Vertex::Hash -> vertex index in the Storage::vertices
		std::unordered_map<uint64_t, size_t> edge_lookup;                 ///< half_edges::Hash -> halfedge index in the Storage::half_edges
		std::unordered_map<size_t, std::vector<size_t>> feature_to_faces; ///< feature_Idx -> list of face indices
	};
} // namespace O::DCEL

#endif // DCEL_STORAGE_H
