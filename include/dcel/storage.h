#ifndef DCEL_STORAGE_H
#define DCEL_STORAGE_H

// STL
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <limits>

// DCEL
#include "face.h"
#include "half_edge.h"
#include "vertex.h"

// UTILS
#include <utils/unowned_ptr.h>

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

		std::unordered_map<uint64_t, O::Unowned_Ptr<Vertex>> vertex_lookup;               ///< Vertex::Hash -> vertex index in the Storage::vertices
		std::unordered_map<uint64_t, O::Unowned_Ptr<Half_Edge>> edge_lookup;                 ///< half_edges::Hash -> halfedge index in the Storage::half_edges
		std::unordered_map<size_t, std::vector<O::Unowned_Ptr<Face>>> feature_to_faces; ///< feature_Idx -> list of face indices


		/**
		 * @brief Create or Get vertex if it does not exist inside the ``vertices``
		 * @param x coordinate of the vertex
		 * @param y coordinate of the vertex
		 * @return size_t index of the vertex in the ``vertices``
		 */
		Vertex& Get_Or_Create_Vertex(double x, double y);

		/**
		 * @brief Create or Get Half_Edge if it does not exist inside the ``half_edges``
		 *        Also ensures the twin exists (but twin may be a separate halfedge created before).
		 * @param origin_vertex the origin ``vertex`` index (in the vertices inside the ``Storage``).
		 * @param head_vertex the head ``vertex`` index (in the vertices inside the ``Storage``).
		 * @return size_t index of the vertex in the ``half_edges``
		 */
		Half_Edge& Get_Or_Create_Half_Edge(Vertex& origin_vertex, Vertex& head_vertex);

		/**
		 * @brief insert half_edge inside the vertext outedge list and directly sort it
		 * 
		 * @param vertex the vertex where to add the outgoing edge
		 * @param edge the edge index that will be added inside the outgoing edge
		 */
		void Insert_Edge_Sorted(const Vertex& vertex, Half_Edge& edge);

		/**
		 * @brief Link the Edge and its twin edge together
		 * @note no check is done if the edge is effectivly the twi because we are in a building process
		 * @param edge one edge
		 * @param twin the twin edge of the edge
		 */
		void Links_twins(Half_Edge& edge, Half_Edge& twin);
		/**
		 * @brief After we added outgoing edges for some vertices, call this to ensure the "around-vertex linking" invariants:
		 *        For each outgoing edge e in clockwise order, set twin(e).next = nextOutgoingEdge and nextOutgoingEdge.prev = twin(e)
		 * @param vertex the vertex where to update all outgoing edge
		 */
		void Update_Around_Vertex(const Vertex& vertex);

		Storage() = delete;
		Storage(size_t vertices_max_num, size_t half_edge_max_num, size_t faces_num);
	};
} // namespace O::DCEL

#endif // DCEL_STORAGE_H
