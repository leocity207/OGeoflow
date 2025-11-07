#include "include/io/full_parser.h"

#include <span>
#include <memory>
#include <cassert>

std::optional<GeoJSON::GeoJSON> GeoJSON::IO::Full_Parser::Get_Geojson()
{
	if(!m_valid) // runonce
		return std::nullopt;
	m_valid = false;
	if(m_is_feature_collection)
		return ::GeoJSON::GeoJSON{::GeoJSON::Feature_Collection{std::move(m_features), std::move(m_bbox), std::move(m_id)}};
	if(m_features.size() == 1)
		return ::GeoJSON::GeoJSON{std::move(m_features.back())};
	else if(m_geometries.size() == 1)
		return ::GeoJSON::GeoJSON{std::move(m_geometries.back())};
	Push_Error(::GeoJSON::IO::Error::Type::UNKNOWN_ROOT_OBJECT);
	return std::nullopt;
}

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
	m_features.emplace_back(std::move(feature));
	assert(m_features.size());
	if(!m_geometries.empty())
		m_features.back().geometry = std::move(m_geometries.back());
	m_geometries.clear();	
	return true;
}

bool GeoJSON::IO::Full_Parser::On_Feature_Collection(std::optional<::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id)
{
	m_bbox = std::move(bbox);
	m_id   = std::move(id);
	m_is_feature_collection = true;
	return true;
}