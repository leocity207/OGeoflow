#ifndef DCEL_HALF_EDGE_H
#define DCEL_HALF_EDGE_H


#include <cstdint>

#include "constant.h"

namespace O::DCEL
{
	/**
	 * @brief Hald_Edge can be seen as directional edges in a graph. one edge between two vertex have two Half_Edge in oposite direction. Hence the Doubly Connected Edge List.
	 */
	struct Half_Edge
	{
		size_t origin = NO_IDX; ///< index of origin Vertex
		size_t twin = NO_IDX; ///< index of twin Half_Edge (reverse direction)
		size_t next = NO_IDX; ///< index of next Half_Edge around face
		size_t prev = NO_IDX; ///< index of previous Half_Edge around face
		size_t face = NO_IDX; ///< index of incident face (left side)

		/**
		 * @brief Create a Unique hash for Half_Edge
		 * @param origin_vertex_index the origin ``vertex`` index (in the vertices inside the ``Storage``).
		 * @param head_vertex_index the head ``vertex`` index (in the vertices inside the ``Storage``).
		 * @return uint64_t
		 */
		static uint64_t Hash(size_t origin_vertex_index, size_t head_vertex_index) noexcept
		{
			return ((uint64_t)origin_vertex_index << 32) | (uint64_t)head_vertex_index;
		}
	};
} // namespace DCEL

#endif // DCEL_HALF_EDGE_H
