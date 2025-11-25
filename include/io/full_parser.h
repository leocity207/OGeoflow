#ifndef IO_FULL_PARSER_H
#define IO_FULL_PARSER_H

#include "sax_parser.h"

namespace O::GeoJSON::IO
{
	/**
	 * @brief A full parser that fully parse the GeoJSON in one Go.
	 * 
	 * User can then retrieve the parsed GeoJSON via the ``Get_Geojson`` function
	 * 
	 * @note While the pasing is done this class hold the whole GeoJSON in memory
	 */
	class Full_Parser : public SAX_Parser<Full_Parser>
	{

	public:
		/**
		 * @brief retirve the fully parsed GeoJSON::Root Object if parsing went well
		 * 
		 * @return std::nullopt if the parsing went bad (``Get_Error``) or if it has been already called function can help the user retrieve what error append
		 * 
		 * @warning this function must only be called once since the O::GeoJSON::Root is moved to the caller
		 */
		std::optional<O::GeoJSON::Root> Get_Geojson();

		/// @name CRTP implementation
		/// @brief Implementation of the Base SAX parser
		/// @{
		bool On_Geometry(O::GeoJSON::Geometry&& geometry, std::size_t element_number);
		bool On_Feature(O::GeoJSON::Feature&& feature);
		bool On_Feature_Collection(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);
		/// @}
	private:

		
		std::vector<O::GeoJSON::Geometry> m_geometries; ///< Temporary geometry accumulator used while parsing.
		std::vector<O::GeoJSON::Feature> m_features;    ///< Temporary feature accumulator used while parsing.
		std::optional<O::GeoJSON::Bbox> m_bbox;         ///< Temporary bbox accumulator used while parsing.
		std::optional<std::string> m_id;                ///< Temporary bbox accumulator used while parsing.
		bool m_is_feature_collection = false;           ///< boolean telling if we were parsing a feature collection. help distinguish one feature inside a feature collection. from just a feature at root
		bool m_valid = true;                            ///< boolean ensuring that the Get_Geojson is called only once
	};
}

#endif //IO_SAX_HANDLER_H