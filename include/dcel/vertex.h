#ifndef DCEL_VERTEX_H
#define DCEL_VERTEX_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <limits>

namespace O::DCEL
{
	struct Vertex
	{
		double x = 0.0;
		double y = 0.0;
		std::vector<size_t> outgoing_edges; // indices into DCEL::halfedges

		uint64_t Hash() const noexcept { return Vertex::Hash(x, y); }
		static uint64_t Hash(double x, double y) noexcept
		{
			// quantize to 1e-6 as in your code
			long long fx = std::llround(x * 1e6);
			long long fy = std::llround(y * 1e6);
			return (static_cast<uint64_t>(static_cast<uint32_t>(fx)) << 32) | static_cast<uint64_t>(static_cast<uint32_t>(fy));
		}
	};

} // namespace O::DCEL

#endif // DCEL_VERTEX_H
