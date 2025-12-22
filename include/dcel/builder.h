#ifndef DCEL_BUILDER_H
#define DCEL_BUILDER_H

// DCEL
#include "storage.h"
#include "feature_info.h"

// GEOJSON
#include "geojson/root.h"
#include "geojson/object/feature.h"
#include "geojson/geometry_type/polygon.h"
#include "geojson/geometry_type/multi_polygon.h"

// UTILS
#include <utils/unowned_ptr.h>

namespace O::DCEL::Builder
{
	/**
	 * @brief Builder is a GeoJSON reciever (from ``IO::Full_Parser``or ``IO::Feature_Parser``) that build a DCEL Structure from the input GeoJSON data
	 */
	class From_GeoJSON
	{
	public:

		From_GeoJSON(const O::Configuration::DCEL& config);
		/**
		 * @brief Create the DCEL structure from a Fully parsed GeoJSON (from IO::Full_Parser for exemple)
		 * @param geojson the full GeoJSON to parse from
		 * @return true or false wether parsing is sucessfull
		 */
		bool Parse(O::GeoJSON::Root&& geojson);


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
		std::vector<O::Unowned_Ptr<Face>> Build_Face_From_Rings(const std::vector<std::vector<O::GeoJSON::Position>>& rings);
		
		/// @name On_Feature
		/// @brief These function are just a extension of On_Full_Feature to dispatch polygone and multipolygones
		/// @{
		bool On_Polygon(const O::GeoJSON::Polygon& poly);
		bool On_MultiPolygon(const O::GeoJSON::Multi_Polygon& mp);
		/// @}

		/**
		 * @brief Create (or Get) the vertex from the given list of position.
		 * @param ring a list of vertex
		 * @return std::vector<size_t> list of index of the created vertex
		 */
		std::vector<O::Unowned_Ptr<Vertex>> Create_Vertex(const std::vector<O::GeoJSON::Position>& ring);

		/**
		 * @brief Create (or Get) the Half_Edge from a given list of ordered linked vertex 
		 * @param ring_vertex list of linked vertex (i-1 is linked with i)
		 * @return std::vector<size_t>  list of index of the created half edges
		 */
		std::vector<O::Unowned_Ptr<Half_Edge>> Create_Forward_Half_Edge(const std::vector<O::Unowned_Ptr<Vertex>>& ring_vertex);

		/**
		 * @brief Link the next and prev for each Half Edge of a ring
		 * @param ring_vertex list of Half Edge in a ring (i-1 is linked with i)
		 */
		void Link_Next_Prev(const std::vector<O::Unowned_Ptr<Half_Edge>>& ring_edges);

		/**
		 * @brief Links a ring of half-edges to a face.
		 *        This function creates a face from a sequence of half-edges forming a closed ring.
		 * @param ring_edge  List of half-edge indices forming the ring.
		 * @param feature_id         ID of the feature associated with this face.
		 * @param is_hole            Whether this ring represents a hole inside the face.
		 * @param outer_face         the outer face that contains this ring.
		 * @return The index of the newly created face.
		 */
		DCEL::Face& Link_Face(std::vector<O::Unowned_Ptr<Half_Edge>>& ring_edge, O::Unowned_Ptr<Face> outer_face);

		void Link_Outer_Bound_Face();


	private:

		Storage m_dcel;    ///< Storage for the DCEL
		bool m_valid_dcel = true;         ///< runonce for the Get_Dcel() function
		bool m_valid_feature_info = true; ///< runonce for the Get_Feature_Info function
		Feature_Info m_feature_info;      ///< Feature_Info to retain the parsed GeoJSON meta data
	};
}

#endif //DCEL_BUILDER_H