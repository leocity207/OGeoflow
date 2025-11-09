#ifndef FILTER_FEATURE_H
#define FILTER_FEATURE_H

#include <utility>
#include <type_traits>
#include <string_view>

#include "include/geojson/object/feature.h"
#include "include/geojson/properties.h"
#include "include/io/feature_parser.h"

namespace GeoJSON::Filter
{

	/**
	 * @brief Feature_Filter
	 *
	 * Template parameters:
	 *  - Next_Handler: type of the next handler/sink. Must expose:
	 *        bool On_Full_Feature(::GeoJSON::Feature&& f);
	 *        bool On_Root(std::optional<::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);
	 *    (On_Root forwarding is optional but provided.)
	 *
	 *  - Predicate: callable with signature bool(const ::GeoJSON::Feature&) or
	 *        bool(const ::GeoJSON::Feature&) noexcept
	 *
	 * Behavior:
	 *  - When parser invokes On_Full_Feature(feature), the predicate is evaluated.
	 *  - If predicate returns true, the feature is forwarded (moved) to m_next. If false, it is dropped.
	 *  - The return value from On_Full_Feature is whatever m_next.On_Full_Feature returns when forwarded,
	 *    or true (continue parsing) when dropped.
	 *
	 * Design notes for low overhead:
	 *  - The filter stores m_next as a reference (no ownership, no indirection costs beyond reference).
	 *  - Predicate is stored by value. Prefer stateless small callables (lambdas, function pointers, small functors).
	 *  - No extra allocations or deep copies of Feature are performed in the matching path: Move semantics are used.
	 */

	template <class Next_Handler, class Predicate>
	class Feature_Filter : public ::GeoJSON::IO::Feature_Parser<Feature_Filter<Next_Handler, Predicate>>
	{
	public:
		Feature_Filter(Next_Handler& next, Predicate pred) noexcept(std::is_nothrow_move_constructible_v<Predicate>)
		: m_next(next), m_pred(std::move(pred)) {}

		// called by Feature_Parser when a full feature is available
		bool On_Full_Feature(::GeoJSON::Feature&& feature)
		{
			if (!m_pred(feature)) return true;
			return m_next.On_Full_Feature(std::move(feature));
		}

		// forward root (bbox/id) if next supports it
		bool On_Root(std::optional<::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id)
		{
			// If Next_Handler has On_Root, forward; otherwise just continue.
			if constexpr (std::is_invocable_r_v<bool, decltype(&Next_Handler::On_Root), Next_Handler, std::optional<::GeoJSON::Bbox>, std::optional<std::string>>)
				return m_next.On_Root(std::move(bbox), std::move(id));
			else
			{
				(void)bbox; (void)id;
				return true;
			}
		}

		const Predicate& Get_Predicator() { return m_pred; };

	private:
		Next_Handler& m_next;
		Predicate m_pred;
	};


	//
	// Helper factory function - deduces template parameters for convenience.
	//
	template <class Next_Handler, class Predicate>
	inline Feature_Filter<Next_Handler, std::decay_t<Predicate>>
	Make_Feature_Filter(Next_Handler& next, Predicate&& pred)
	{
		return Feature_Filter<Next_Handler, std::decay_t<Predicate>>(next, std::forward<Predicate>(pred));
	}

}

#endif // FILTER_FEATURE_H