#ifndef DCEL_BUILDER_H
#define DCEL_BUILDER_H


#include "storage_builder_helper.h"

#include "include/geojson/root.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/geometry_type/polygon.h"
#include "include/geojson/geometry_type/multi_polygon.h"
#include "feature_info.h"

namespace O::DCEL
{
	/**
	 * @brief Builder is a GeoJSON reciever (from ``IO::Full_Parser``or ``IO::Feature_Parser``) that build a DCEL Structure from the input GeoJSON data
	 */
	class Builder
	{
	public:
		/**
		 * @brief Create the DCEL structure from a Fully parsed GeoJSON (from IO::Full_Parser for exemple)
		 * @param geojson the full GeoJSON to parse from
		 * @return true or false wether parsing is sucessfull
		 */
		bool Parse_GeoJSON(O::GeoJSON::Root&& geojson);


		/// @name overrides
		/// @brief implementation of the O::GeoJSON::IO::Feature_Parser functions.
		/// @{
		bool On_Full_Feature(O::GeoJSON::Feature&& feature);
		bool On_Root(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);
		/// @}

		/**
		 * @brief retirve the fully parsed ``DCEL::Storage`` Object if parsing went well
		 * @return DCEL::Storage Object, std::nullopt if the parsing went bad or if already been called. 
		 * @warning this function must only be called once since the ``DCEL::Storage`` is moved to the caller
		 */
		std::optional<Storage> Get_Dcel();

		/**
		 * @brief retirve the fully parsed ``DCEL::Feature_Info`` Object if parsing went well
		 * @return DCEL::Feature_Info Object, std::nullopt if the parsing went bad or if already been called. 
		 * @warning this function must only be called once since the ``DCEL::Feature_Info`` is moved to the caller
		 */
		std::optional<Feature_Info> Get_Feature_Info();

	private:
		/**
		 * @brief Builds a new ``Face`` from the previously generated vertex rings.
		 * @param rings The list of rings, each represented as a sequence of ``O::GeoJSON::Position`` objects. Nested rings are treated as holes inside the face.
		 * @param feature_id The ID of the feature associated with the resulting face.
		 */
		void Build_Face_From_Rings(const std::vector<std::vector<O::GeoJSON::Position>>& rings, size_t feature_id);
		
		/// @name On_Feature
		/// @brief These function are just a extension of On_Full_Feature to dispatch polygone and multipolygones
		/// @{
		bool On_Polygon(const O::GeoJSON::Polygon& poly, size_t feature_id);
		bool On_MultiPolygon(const O::GeoJSON::Multi_Polygon& mp, size_t feature_id);
		/// @}

		/**
		 * @brief Create (or Get) the vertex from the given list of position.
		 * @param ring a list of vertex
		 * @return std::vector<size_t> list of index of the created vertex
		 */
		std::vector<size_t> Create_Vertex(const std::vector<O::GeoJSON::Position>& ring);

		/**
		 * @brief Create (or Get) the Half_Edge from a given list of ordered linked vertex 
		 * @param ring_vertex_indices list of linked vertex (i-1 is linked with i)
		 * @return std::vector<size_t>  list of index of the created half edges
		 */
		std::vector<size_t> Create_Forward_Half_Edge(const std::vector<size_t>& ring_vertex_indices);

		/**
		 * @brief Link the next and prev for each Half Edge of a ring
		 * @param ring_vertex_indices list of Half Edge in a ring (i-1 is linked with i)
		 */
		void Link_Next_Prev(const std::vector<size_t>& ring_edge_indices);

		/**
		 * @brief Links a ring of half-edges to a face.
		 *        This function creates a face from a sequence of half-edges forming a closed ring.
		 * @param ring_edge_indices  List of half-edge indices forming the ring.
		 * @param feature_id         ID of the feature associated with this face.
		 * @param is_hole            Whether this ring represents a hole inside the face.
		 * @param outer_face_id      ID of the outer face that contains this ring.
		 * @return The index of the newly created face.
		 */
		size_t Link_Face(const std::vector<size_t>& ring_edge_indices, size_t feature_id, bool is_hole, size_t outer_face_id);


	private:

		Storage_Builder_Helper m_dcel;    ///< Storage for the DCEL
		bool m_valid_dcel = true;         ///< runonce for the Get_Dcel() function
		bool m_valid_feature_info = true; ///< runonce for the Get_Feature_Info function
		Feature_Info m_feature_info;      ///< Feature_Info to retain the parsed GeoJSON meta data
	};
}

#endif //DCEL_BUILDER_H