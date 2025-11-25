#ifndef DCEL_DCEL_BUILDER_H
#define DCEL_DCEL_BUILDER_H

#include "storage.h"

namespace O::DCEL
{

    struct Storage_Builder_Helper : public Storage
    {
        /// @brief Create-or-get vertex, returns index
        size_t Get_Or_Create_Vertex(double x, double y);

        /// @brief Compose edge key (originIdx, headIdx) -> 64bit
        static uint64_t EdgeKey(size_t originIdx, size_t headIdx) noexcept;

        /// @brief Add half-edge for origin->head if not present. Returns halfedge index.
        ///        Also ensures the twin exists (but twin may be a separate halfedge created before).
        size_t Get_Or_Create_Half_Edge_Index(size_t origin_id, size_t head_id);

		/// @brief insert half_edge inside the vertext outedge in ordered
		void Insert_Edge_Sorted(size_t vertex_id, size_t edge_id);

        void Links_twins(size_t edge, size_t twin);

        // After we added outgoing edges for some vertices, call this to ensure the "around-vertex linking" invariants:
        // For each outgoing edge e in clockwise order, set twin(e).next = nextOutgoingEdge and nextOutgoingEdge.prev = twin(e)
        void Update_Around_Vertex(size_t vertexIdx);
    };
} // namespace DCEL

#endif // DCEL_DCEL_BUILDER_H
