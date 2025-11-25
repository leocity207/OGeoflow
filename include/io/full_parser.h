#ifndef IO_FULL_PARSER_H
#define IO_FULL_PARSER_H

#include "sax_parser.h"

namespace O::GeoJSON::IO
{
	class Full_Parser : public SAX_Parser<Full_Parser>
	{
	public:
		std::optional<O::GeoJSON::Root> Get_Geojson();
	public:
		bool On_Geometry(O::GeoJSON::Geometry&& geometry, std::size_t element_number);
		bool On_Feature(O::GeoJSON::Feature&& feature);
		bool On_Feature_Collection(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);
	private:
		std::vector<O::GeoJSON::Geometry> m_geometries;
		std::vector<O::GeoJSON::Feature> m_features;
		std::optional<O::GeoJSON::Bbox> m_bbox;
		std::optional<std::string> m_id;
		bool m_is_feature_collection = false;
		bool m_valid = true;
	};
}

#endif //IO_SAX_HANDLER_H