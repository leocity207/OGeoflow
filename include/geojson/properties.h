#ifndef GEOJSON_PROPERTIES_H
#define GEOJSON_PROPERTIES_H

#include <vector>
#include <map>
#include <variant>
#include <string>
#include <functional>

namespace O::GeoJSON 
{
	struct Property {
		using Array = std::vector<Property>;
		using Object = std::map<std::string, Property>;
		std::variant<
			std::monostate,
			bool,
			std::int64_t,
			double,
			std::string,
			Array,
			Object
		> m_value;

		/// @brief Contruction
		Property()               noexcept = default;
		Property(std::nullptr_t) noexcept : m_value(std::monostate{}) {}
		Property(bool b)         noexcept : m_value(b) {}
		Property(std::int64_t i) noexcept : m_value(i) {}
		Property(int i)          noexcept : m_value(static_cast<std::int64_t>(i)) {}
		Property(double d)       noexcept : m_value(d) {}
		Property(const char* s)           : m_value(std::string(s)) {}
		Property(std::string s)           : m_value(std::move(s)) {}
		Property(Array&& a)               : m_value(std::move(a)) {}
		Property(Object&& o)              : m_value(std::move(o)) {}

		/// @brief copy/move defaulted
		Property(const Property&) = default;
		Property(Property&&) noexcept = default;
		Property& operator=(const Property&) = default;
		Property& operator=(Property&&) noexcept = default;

		/// @brief Assign from supported types (strongly typed)
		Property& operator=(std::nullptr_t)       noexcept { m_value = std::monostate{};             return *this; }
		Property& operator=(bool b)               noexcept { m_value = b;                            return *this; }
		Property& operator=(std::int64_t i)       noexcept { m_value = i;                            return *this; }
		Property& operator=(int i)                noexcept { m_value = static_cast<std::int64_t>(i); return *this; }
		Property& operator=(double d)             noexcept { m_value = d;                            return *this; }
		Property& operator=(const std::string& s) noexcept { m_value = s;                            return *this; }
		Property& operator=(std::string&& s)      noexcept { m_value = std::move(s);                 return *this; }
		Property& operator=(Array&& a)            noexcept { m_value = std::move(a);                 return *this; }
		Property& operator=(Object&& o)           noexcept { m_value = std::move(o);                 return *this; }

		/// @brief check queries
		bool Is_Null()    const noexcept { return std::holds_alternative<std::monostate>(m_value); }
		bool Is_Bool()    const noexcept { return std::holds_alternative<bool>(m_value); }
		bool Is_Integer() const noexcept { return std::holds_alternative<std::int64_t>(m_value); }
		bool Is_Double()  const noexcept { return std::holds_alternative<double>(m_value); }
		bool Is_String()  const noexcept { return std::holds_alternative<std::string>(m_value); }
		bool Is_Array()   const noexcept { return std::holds_alternative<Array>(m_value); }
		bool Is_Object()  const noexcept { return std::holds_alternative<Object>(m_value); }

		/// @brief scalar type
		bool         Get_Bool()   const noexcept { return std::get<bool>(m_value); }
		std::int64_t Get_Int()    const noexcept { return std::get<std::int64_t>(m_value); }
		double       Get_Double() const noexcept { return std::get<double>(m_value); }

		/// @brief const queries
		const std::string_view  Get_String() const noexcept { return std::get<std::string>(m_value); }
		const Array&            Get_Array()  const noexcept { return std::get<Array>(m_value); }
		const Object&           Get_Object() const noexcept { return std::get<Object>(m_value); }

		/// @brief mutable queries
		std::string_view  Get_String()  noexcept { return std::get<std::string>(m_value); }
		Array&            Get_Array()   noexcept { return std::get<Array>(m_value); }
	 	Object&           Get_Object()  noexcept { return std::get<Object>(m_value); }

		std::size_t Size() const noexcept
		{
			return std::visit([](auto&& val) -> std::size_t {
				using T = std::decay_t<decltype(val)>;
				if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Object>)
					return val.size();
				else
					return 0;
			}, m_value);
		}

		void Clear() noexcept
		{
			std::visit([](auto&& val) {
				using T = std::decay_t<decltype(val)>;
				if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Object>)
					val.clear();
				else if constexpr (!std::is_same_v<T, std::monostate>)
					val = T{}; // reset type to default
			}, m_value);
		}
	};
}

#endif // GEOJSON_PROPERTIES_H