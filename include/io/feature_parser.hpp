#include "include/io/full_parser.h"

#include <span>
#include <memory>
#include <cassert>

bool GeoJSON::IO::Full_Parser::On_Geometry(::GeoJSON::Geometry&& geometry, std::size_t element_number)
{
	if(geometry.Is_Geometry_Collection() && element_number > 0)
	{
		::GeoJSON::Geometry_Collection collection;
		if(m_geometries.size() < element_number) return Push_Error(::GeoJSON::IO::Error::Type::GEOMETRy_COLLECTION_ELLEMENT_COUNT_MISMATCH);
		for (auto& temp_geometry : std::span<::GeoJSON::Geometry>(m_geometries.data() + (m_geometries.size() - element_number),element_number))
			collection.geometries.emplace_back(std::make_shared<::GeoJSON::Geometry>(std::move(temp_geometry)));
		geometry.value = std::move(collection);
		m_geometries.erase(m_geometries.end() - element_number, m_geometries.end());
	}
	m_geometries.emplace_back(std::move(geometry));
	return true;
}

bool GeoJSON::IO::Full_Parser::On_Feature(::GeoJSON::Feature&& feature)
{
	if(!m_geometries.empty())
		feature.geometry = std::move(m_geometries.back());
	On_Full_Feature(std::move(feature));
	return true;
}

bool GeoJSON::IO::Full_Parser::On_Feature_Collection(std::optional<::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id)
{
	m_bbox = std::move(bbox);
	m_id   = std::move(id);
	m_is_feature_collection = true;
	return true;
}