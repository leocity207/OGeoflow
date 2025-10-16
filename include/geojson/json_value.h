#ifndef GEOJSON_JSON_VALUE_H
#define GEOJSON_JSON_VALUE_H

#include <vector>
#include <map>
#include <variant>
#include <string>

namespace GeoJSON 
{
	struct JSON_Value;
	using JSON_Array = std::vector<JSON_Value>;
	using JSON_Object = std::map<std::string, JSON_Value>;

	struct JSON_Value {
		using Variant_Type = std::variant<
			std::monostate,   // null
			bool,
			double,
			std::string,
			JSON_Array,
			JSON_Object
		>;

		Variant_Type value;

		JSON_Value() = default;
		template<typename T>
			requires std::disjunction_v<
				std::is_same<std::decay_t<T>, bool>,
					std::is_same<std::decay_t<T>, double>,
					std::is_same<std::decay_t<T>, std::string>,
					std::is_same<std::decay_t<T>, JSON_Array>,
					std::is_same<std::decay_t<T>, JSON_Object>
			>
		JSON_Value& operator=(T&& v) noexcept {
			value = std::forward<T>(v);
			return *this;
		}

		// Copy & move
		JSON_Value(const JSON_Value&) = default;
		JSON_Value(JSON_Value&&) noexcept = default;
		JSON_Value& operator=(const JSON_Value&) = default;
		JSON_Value& operator=(JSON_Value&&) noexcept = default;

		// Generic assignment operator for any supported type
		template<typename T>
		JSON_Value& operator=(T&& v) noexcept {
			value = std::forward<T>(v);
			return *this;
		}

		bool is_null() const;
		bool is_bool() const;
		bool is_number() const;
		bool is_string() const;
		bool is_array() const;
		bool is_object() const;

		const JSON_Object& as_object() const;
		const JSON_Array& as_array() const;
		const std::string& as_string() const;
		double as_number() const;
		bool as_bool() const;
	};
}

#endif //GEOJSON_JSON_VALUE_H