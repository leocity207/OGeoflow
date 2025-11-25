#ifndef FILTER_FEATURE_HPP
#define FILTER_FEATURE_HPP

#include "feature.h"

template <class Next_Handler, class Predicate>
requires std::invocable<Predicate, O::GeoJSON::Feature&>
O::GeoJSON::Filter::Feature<Next_Handler, Predicate>::Feature(Predicate pred) : 
	m_pred(std::move(pred)) 
{

}

template <class Next_Handler, class Predicate>
requires std::invocable<Predicate, O::GeoJSON::Feature&>
bool O::GeoJSON::Filter::Feature<Next_Handler, Predicate>::On_Full_Feature(O::GeoJSON::Feature&& feature)
{
	if constexpr (!requires(Next_Handler d) { d.On_Full_Feature(std::move(feature)); })
		static_assert(false,"Derived must implement: bool On_Full_Feature(Feature&&)");
	if (!m_pred(feature)) return true;
	return static_cast<Next_Handler&>(*this).On_Full_Feature(std::move(feature));
}

template <class Next_Handler, class Predicate>
requires std::invocable<Predicate, O::GeoJSON::Feature&>
bool O::GeoJSON::Filter::Feature<Next_Handler, Predicate>::On_Root(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id)
{
	if constexpr (!requires(Next_Handler d) { d.On_Root(std::move(bbox), std::move(id)); })
		static_assert(false,"Derived must implement: bool On_Root(optional<Bbox>&&, optional<string>&&)");
	return static_cast<Next_Handler&>(*this).On_Root(std::move(bbox), std::move(id));
}

template <class Next_Handler, class Predicate>
requires std::invocable<Predicate, O::GeoJSON::Feature&>
const Predicate& O::GeoJSON::Filter::Feature<Next_Handler, Predicate>::Get_Predicator() 
{ 
	return m_pred;
};


#endif // FILTER_FEATURE_H