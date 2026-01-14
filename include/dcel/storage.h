#ifndef DCEL_STORAGE_H
#define DCEL_STORAGE_H

// STL
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <limits>

// CONFIGURATION
#include "configuration/dcel.h"

// UTILS
#include <utils/unowned_ptr.h>

namespace O::DCEL
{
	/**
	 * @brief Storage is structure that hold all the DCEL Information (``Vertex``, ``Half_Edge``, ``Face``).
	 *        The class also contain look up map for fast acess to linked feature.
	 *        We also embed a look up table that link face to their feature inside the GeoJSON it can be linked to the ``Feature_Info`` class
	 */
	template<class Vertex, class Half_Edge, class Face>
	struct Storage
	{
		O::Configuration::DCEL config;
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
		 * @brief tell if a vertex already exist at coordinate
		 * @param x coordinate of the vertex
		 * @param y coordinate of the vertex
		 * @return true if a vertex exist
		 * @return false vertex does not exist
		 */
		bool Does_Vertex_Exist(double x, double y);

		/**
		 * @brief Create or Get Half_Edge if it does not exist inside the ``half_edges``
		 *        Also ensures the twin exists (but twin may be a separate halfedge created before).
		 * @param origin_vertex the origin ``vertex`` (in the vertices inside the ``Storage``).
		 * @param head_vertex the head ``vertex`` (in the vertices inside the ``Storage``).
		 * @return size_t index of the vertex in the ``half_edges``
		 */
		Half_Edge& Get_Or_Create_Half_Edge(Vertex& origin_vertex, Vertex& head_vertex);

		/**
		 * @brief tell if an half edge linking the two vertex exist
		 * @param x the origin ``vertex`` (in the vertices inside the ``Storage``).
		 * @param y the head ``vertex`` (in the vertices inside the ``Storage``).
		 * @return true if a half edge exist
		 * @return false half edge does not exist
		 */
		bool Does_Half_Edge_Exist(Vertex& origin_vertex, Vertex& head_vertex);

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
		Storage(const O::Configuration::DCEL& config);

		bool Move(Vertex& edge, double new_x, double new_y);

		private:

			/**
			 * @brief remove the half_edge and its twin by swaping it with the last half_edge
			 * @param edge edge and its associated twin to remove
			 * @return true if removing was successfull
			 * @return false if removing failed
			 * @note care should be taken because this function does not check if the given edge was used in the storage you should merge it before remove it
			 */
			bool Remove(Half_Edge& edge);

			/**
			 * @brief remove the vertex by swaping it with the last vertex in the storage
			 * @param vertex to remove
			 * @return true if removing was successfull
			 * @return false if removing failed
			 */
			bool Remove(Vertex& vertex);

			bool Merge(Vertex& v_keep, Vertex& v_discard, Half_Edge& e_discard);

	};
} // namespace O::DCEL

#include "storage.hpp"

#endif // DCEL_STORAGE_H
