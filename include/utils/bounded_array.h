// File: utils/bounded_array.h
#ifndef UTILS_BOUNDED_ARRAY_H
#define UTILS_BOUNDED_ARRAY_H

#include <array>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <iterator>
#include <algorithm>

namespace O
{
	/**
	 * @brief A fixed-capacity vector-like container with no dynamic allocation.
	 * 	      This class provides a `std::vector`-like interface but stores all elements in a preallocated, statically-sized buffer. 
	 *        Elements are constructed in-place and destroyed manually. 
	 *        Attempts to exceed capacity or access out-of-range elements trigger a simple typed exception.
	 * 
	 * @tparam T Type of stored elements.
	 * @tparam N Maximum number of elements the container can hold.
	 */
	template<typename T, std::size_t N>
	class Bounded_Vector 
	{
	public:
		/**
		 * @brief Lightweight exception payload used by member functions.
		 *        Only contains a `Type` discriminant; users may inspect the `type` field to determine the cause (out-of-range access, push on full container, pop on empty container).
		 */
		struct Exception
		{
			enum class Type
			{
				NO_ERROR = 0,
				OUT_OF_RANGE,
				NO_MORE_ELEMENT_TO_POP,
				NO_MORE_PLACE_TO_EMPLACE
			};
			Type type;
		};
	private:
		// storage: uninitialized, properly aligned memory for up to N objects of T
		using storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;
		std::array<storage_t, N> m_buffer; ///< Full stdarray buffer
		std::size_t m_size = 0;            ///< Effective size of the bounded buffer

		/** @name Internal helpers
		 *  @brief Raw pointer accessors into the internal buffer.
		 *         These functions convert the raw storage slot into a typed pointer.
		 *  @{ */
		T* Ptr_At(std::size_t i) noexcept;
		const T* Ptr_At(std::size_t i) const noexcept;
		/** @} */

	public:
		/** @name Construction / assignment
		 *  @brief Default, copy, move, destructor and assignment behave similarly to std::vector:
		 *
		 *  - Copy operations copy-construct each live element into the destination's
		 *    internal storage.
		 *  - Move operations move-construct elements and leave the moved-from
		 *    container empty.
		 *  - The destructor destroys all live elements.
		 *  @{ */
		constexpr Bounded_Vector() noexcept = default;
		~Bounded_Vector() noexcept;
		Bounded_Vector(const Bounded_Vector& other);
		Bounded_Vector(Bounded_Vector&& other) noexcept(std::is_nothrow_move_constructible<T>::value);
		Bounded_Vector& operator=(const Bounded_Vector& other);
		Bounded_Vector& operator=(Bounded_Vector&& other) noexcept(std::is_nothrow_move_constructible<T>::value);
		/** @} */

		/** @name Capacity / state
		 *  @brief Query capacity and size. Capacity() is constexpr and always equals N.
		 *  @{ */
		constexpr std::size_t Capacity() const noexcept;
		std::size_t Size() const noexcept;
		bool Empty() const noexcept;
		bool Full() const noexcept;
		/** @} */

		/** @name Element access
		 *  @brief Random-access accessors.
		 *  @{ */
		T& operator[](std::size_t i) noexcept;
		const T& operator[](std::size_t i) const noexcept;
		T& At(std::size_t i);
		const T& At(std::size_t i) const;
		T* Data() noexcept;
		const T* Data() const noexcept;
		/** @} */

		/** @name Modifiers
		 *  @brief Mutating operations.
		 *  @{ */
		template<class... Args>
		void Emplace_Back(Args&&... args);
		void Push_Back(const T& v);
		void Push_Back(T&& v);
		void Pop_Back();
		void Clear() noexcept;
		/** @} */

		/** @name Iterators
		 *  @brief Pointer-style contiguous iterators compatible with standard algorithms.
		 *  @{ */
		T* begin() noexcept;
		T* end() noexcept;
		const T* begin() const noexcept;
		const T* end() const noexcept;
		const T* cbegin() const noexcept;
		const T* cend() const noexcept;
		/** @} */
	};
}

#include "bounded_array.hpp"

#endif //UTILS_BOUNDED_ARRAY_H
