#ifndef FILTER_FEATURE_H
#define FILTER_FEATURE_H

#include <concepts>

#include "include/geojson/object/feature.h"
#include "include/io/feature_parser.h"

namespace O::GeoJSON::Filter
{
	/**
	* @brief a default predicate that always return true
	*/
	struct Default_Predicate { constexpr bool operator()(O::GeoJSON::Feature&) const noexcept {return true;}};

	/**
	 * @brief Feature_Filter
	 * 
	 * Behavior:
	 *  - When parser invokes On_Full_Feature(feature), the predicate is evaluated.
	 *  - If predicate returns true, the feature is forwarded (moved) to m_next. If false, it is dropped.
	 *  - The return value from On_Full_Feature is whatever m_next.On_Full_Feature returns when forwarded, or true (continue parsing) when dropped.
	 *
	 * @tparam Next_Handler: type of the next handler/sink. Must expose:
	 *        bool On_Full_Feature(::GeoJSON::Feature&& f);
	 *        bool On_Root(std::optional<::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);
	 * 
	 * @tparam Predicate: callable with signature bool(const ::GeoJSON::Feature&) the predicate can be default or non default constructible
	 *

	 *
	 * @note Design notes for low overhead:
	 *  - The filter use CRTP to Call Next_Handler value.
	 *  - Predicate is stored by value. Prefer stateless small callables (lambdas, function pointers, small functors).
	 */
	template <class Next_Handler, class Predicate = Default_Predicate>
	requires std::invocable<Predicate, O::GeoJSON::Feature&>
	class Feature : public O::GeoJSON::IO::Feature_Parser<Feature<Next_Handler, Predicate>>
	{
	public:

		/**
		 * @brief default constructor
		 * @note only available when m_pred is default constructible
		 */
		Feature() requires std::default_initializable<Predicate> : m_pred() {};

		/**
		 * @brief Constructor with a predicate
		 * @param pred the predicate to use when evaluating 
		 */
		Feature(Predicate pred);
		
		/// @name overrides
		/// @brief implementation of the ``O::GeoJSON::IO::Feature_Parser`` functions
		/// @{
		bool On_Full_Feature(O::GeoJSON::Feature&& feature);
		bool On_Root(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id);
		/// @}

		/**
		 * @brief give the filter predicator
		 * @return the instance predicator
		 */
		const Predicate& Get_Predicator();

	private:
		Predicate m_pred;
	};

}

#include "feature.hpp"

#endif // FILTER_FEATURE_H