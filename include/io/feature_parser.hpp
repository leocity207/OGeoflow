#include "include/io/feature_parser.h"

#include <span>
#include <memory>
#include <cassert>

template <class Derived>
bool O::GeoJSON::IO::Feature_Parser<Derived>::On_Full_Feature(O::GeoJSON::Feature&& feature) 
{
	if constexpr (!requires(Derived d) { d.On_Full_Feature(std::move(feature)); })
		static_assert(false,"Derived must implement:\n    bool On_Full_Feature(Feature&&)");
	return static_cast<Derived&>(*this).On_Full_Feature(std::move(feature));
}

template <class Derived>
bool O::GeoJSON::IO::Feature_Parser<Derived>::On_Root(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id) 
{
	if constexpr (!requires(Derived d) { d.On_Root(std::move(bbox), std::move(id)); })
		static_assert(false,"Derived must implement:\n    bool On_Root(optional<Bbox>&&, optional<string>&&)");
	return static_cast<Derived&>(*this).On_Root(std::move(bbox), std::move(id));
}

template <class Derived>
bool O::GeoJSON::IO::Feature_Parser<Derived>::On_Geometry(O::GeoJSON::Geometry&& geometry, std::size_t element_number)
{
	if(geometry.Is_Geometry_Collection() && element_number > 0)
	{
		O::GeoJSON::Geometry_Collection collection;
		if(m_geometries.size() < element_number) return  O::GeoJSON::IO::SAX_Parser<O::GeoJSON::IO::Feature_Parser<Derived>>::Push_Error(O::GeoJSON::IO::Error::GEOMETRY_COLLECTION_ELLEMENT_COUNT_MISMATCH);
		for (auto& temp_geometry : std::span<O::GeoJSON::Geometry>(m_geometries.data() + (m_geometries.size() - element_number),element_number))
			collection.geometries.emplace_back(std::make_shared<O::GeoJSON::Geometry>(std::move(temp_geometry)));
		geometry.value = std::move(collection);
		m_geometries.erase(m_geometries.end() - element_number, m_geometries.end());
	}
	m_geometries.emplace_back(std::move(geometry));
	return true;
}

template <class Derived>
bool O::GeoJSON::IO::Feature_Parser<Derived>::On_Feature(O::GeoJSON::Feature&& feature)
{
	if(!m_geometries.empty())
		feature.geometry = std::move(m_geometries.back());
	On_Full_Feature(std::move(feature));
	return true;
}