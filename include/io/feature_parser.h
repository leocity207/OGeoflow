#ifndef IO_FEATURE_PARSER_H
#define IO_FEATURE_PARSER_H

#include "sax_parser.h"

namespace GeoJSON::IO
{
	template <class Derived>
	class Feature_Parser : public SAX_Parser<Feature_Parser<Derived>>
	{
	public:
		bool On_Full_Feature(::GeoJSON::Feature&& feature) {return static_cast<Derived&>(*this).On_Full_Feature(std::move(feature));};
		bool On_Root(std::optional<::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id) {return static_cast<Derived&>(*this).On_Root(std::move(bbox), std::move(id));};


		bool On_Geometry(::GeoJSON::Geometry&& geometry, std::size_t element_number);
		bool On_Feature(::GeoJSON::Feature&& feature);
		bool On_Feature_Collection(std::optional<::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id) {return On_Root(std::move(bbox),std::move(id));};

	private:
		std::vector<::GeoJSON::Geometry> m_geometries;
		bool m_is_feature_collection = false;
		bool m_valid = true;
	};
}

#include "feature_parser.hpp"

#endif //IO_FEATURE_PARSER_H