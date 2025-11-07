#ifndef IO_FULL_PARSER_H
#define IO_FULL_PARSER_H

#include "sax_parser.h"

namespace GeoJSON::IO
{
	class Full_Parser : public SAX_Parser<Full_Parser>
	{
	public:
		std::optional<::GeoJSON::GeoJSON> Get_Geojson();
	public:
		bool On_Geometry(::GeoJSON::Geometry&& geometry, std::size_t element_number);
		bool On_Feature(::GeoJSON::Feature&& feature);
		bool On_Feature_Collection(std::optional<::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);
	private:
		std::vector<::GeoJSON::Geometry> m_geometries;
		std::vector<::GeoJSON::Feature> m_features;
		std::optional<::GeoJSON::Bbox> m_bbox;
		std::optional<::std::string> m_id;
		bool m_is_feature_collection = false;
		bool m_valid = true;
	};
}

#endif //IO_SAX_HANDLER_H