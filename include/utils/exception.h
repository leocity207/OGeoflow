#ifndef UTILS_EXCPETION_H
#define UTILS_EXCEPTION_H


namespace Util
{
	/**
	 * @brief Exception class for GeoJSON parsing errors
	 */
	struct Exception
	{
	};

	/**
	 * @brief Exception class for GeoJSON parsing errors
	 */
	template<class TException, class Type >
	inline void Ensure(bool condition, Type type)
	{
		if(!condition)
			throw TException{type};
	}
}

#endif