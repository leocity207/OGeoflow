#ifndef DCEL_DCEL_H
#define DCEL_DCEL_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <limits>

namespace DCEL
{
	constexpr size_t NO_IDX = std::numeric_limits<size_t>::max();

	struct Half_Edge;
	struct Face;

	struct Vertex
	{
		double x = 0.0;
		double y = 0.0;
		std::vector<size_t> outgoing_edges; // indices into DCEL::halfedges

		uint64_t Hash() const noexcept { return Vertex::Hash(x,y); }
		static uint64_t Hash(double x, double y) noexcept
		{
			// quantize to 1e-6 as in your code
			long long fx = std::llround(x * 1e6);
			long long fy = std::llround(y * 1e6);
			return (static_cast<uint64_t>(static_cast<uint32_t>(fx)) << 32) | static_cast<uint64_t>(static_cast<uint32_t>(fy));
		}
	};

	struct Half_Edge
	{
		size_t origin = NO_IDX; // index of origin vertex
		size_t twin   = NO_IDX; // index of twin halfedge (reverse direction)
		size_t next   = NO_IDX; // index of next halfedge around face
		size_t prev   = NO_IDX; // index of previous halfedge around face
		size_t face   = NO_IDX; // index of incident face (left side)
	};

	struct Face
	{
		size_t edge = NO_IDX; // a representative halfedge belonging to this face
		size_t associated_feature = NO_IDX; // indices of features that reference this face
		std::vector<size_t> inner_edges; // representative edges for holes (if any)

    	size_t outer_face = NO_IDX;     
	};

	struct DCEL
	{
		std::unordered_map<uint64_t, size_t> vertex_lookup; // hash -> vertex index
		std::vector<Vertex> vertices;
		std::unordered_map<uint64_t, size_t> edge_lookup; // (origin<<32 | head) -> halfedge index
		std::vector<Half_Edge> halfedges;
		std::vector<Face> faces;

    	std::unordered_map<size_t, std::vector<size_t>> feature_to_faces; // featureIdx -> list of face indices
	};
} // namespace DCEL

#endif // DCEL_DCEL_H
