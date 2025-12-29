#ifndef DCEL_HALF_EDGE_H
#define DCEL_HALF_EDGE_H

// STL
#include <cstdint>

// UTILS
#include <utils/unowned_ptr.h>

namespace O::DCEL
{
	/**
	 * @brief Hald_Edge can be seen as directional edges in a graph. one edge between two vertex have two Half_Edge in oposite direction. Hence the Doubly Connected Edge List.
	 */
	template<class Vertex, class Half_Edge_T, class Face>
	struct Half_Edge
	{
		O::Unowned_Ptr<Vertex> tail = nullptr; ///< index of origin Vertex
		O::Unowned_Ptr<Vertex> head = nullptr; ///< index of origin Vertex
		O::Unowned_Ptr<Half_Edge_T> twin = nullptr; ///< index of twin Half_Edge (reverse direction)
		O::Unowned_Ptr<Half_Edge_T> next = nullptr; ///< index of next Half_Edge around face
		O::Unowned_Ptr<Half_Edge_T> prev = nullptr; ///< index of previous Half_Edge around face
		O::Unowned_Ptr<Face> face = nullptr; ///< index of incident face (left side)

		/**
		 * @brief Create a Unique hash for Half_Edge
		 * @param origin_vertex_index the origin ``vertex`` index (in the vertices inside the ``Storage``).
		 * @param head_vertex_index the head ``vertex`` index (in the vertices inside the ``Storage``).
		 * @return uint64_t
		 */
		static uint64_t Hash(const Vertex& origin_vertex,const Vertex& head_vertex) noexcept
		{
			uintptr_t a = reinterpret_cast<uintptr_t>(&origin_vertex);
			uintptr_t b = reinterpret_cast<uintptr_t>(&head_vertex);

			uint64_t h = a;
			h ^= b + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);

			return h;
		}

		Half_Edge(Vertex& tail,Vertex& head) :
			tail(&tail),
			head(&head),
			twin(nullptr),
			next(nullptr),
			prev(nullptr),
			face(nullptr)
		{

		}

		bool operator==(const Half_Edge& other) const
		{
			return Hash(*tail, *head) == Hash(*other.tail, *other.head);
		}
	};
} // namespace DCEL

#endif // DCEL_HALF_EDGE_H
