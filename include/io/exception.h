#ifndef IO_EXCEPTION_H
#define IO_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace IO
{
	/**
	 * @brief Exception class for GeoJSON parsing errors
	 */
	class Exception : public std::runtime_error 
	{
		public:
			explicit Exception(const std::string& message) : std::runtime_error(message) {}
	};

	/**
	 * @brief Exception class for GeoJSON parsing errors
	 */
	inline void Ensure(bool condition,const std::string& message)
	{
		if(!condition)
			throw Exception(message);
	}
}

#endif //IO_EXCEPTION_H