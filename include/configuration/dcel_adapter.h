#ifndef INCLUDE_CONFIGURATION_VECTOR_ADAPTER_H
#define INCLUDE_CONFIGURATION_VECTOR_ADAPTER_H

// STL
#include <configuration/module/json_builder.h>
#include <configuration/module/json_writer.h>
#include <configuration/module/traits.h>


// CONFIGURATION
#include "dcel.h"

namespace O::Configuration
{
	enum class Error
	{
		POINT_PRECISION_SHOULD_BE_DOUBLE,
		MAX_SHOULD_BE_UNSIGNED_VALUE,
		MERGE_STRATEGY_SHOULD_BE_STRING,
		MERGE_STRATEGY_UNKNOWN
	};

	struct DCEL_Adapter : public O::Configuration::Module::JSON_Builder<DCEL_Adapter, DCEL, Error>
	{
		std::optional<Error> Load_From_JSON(const rapidjson::Value& v);

		template<class RapidJSON_Writer>
		void To_JSON(RapidJSON_Writer& writer, const DCEL& data) const;

		static constexpr const char* Key() noexcept {return "DCEL";};
	};
	
} // O::Configuration

template<>
struct O::Configuration::Module::Traits<O::Configuration::DCEL>
{
	using Builder = O::Configuration::DCEL_Adapter;
	using Writer = O::Configuration::DCEL_Adapter;
};

#endif //INCLUDE_CONFIGURATION_VECTOR_ADAPTER_H