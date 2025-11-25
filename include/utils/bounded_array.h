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
	template<typename T, std::size_t N>
	class Bounded_Vector 
	{
	public:
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
		// stockage non-initialisé, correctement aligné pour T
		using storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;
		std::array<storage_t, N> buffer_;
		std::size_t m_size = 0;

		// helper: pointeur vers un emplacement i
		T* Ptr_At(std::size_t i) noexcept
		{
			return reinterpret_cast<T*>(&buffer_[i]);
		}
		const T* Ptr_At(std::size_t i) const noexcept
		{
			return reinterpret_cast<const T*>(&buffer_[i]);
		}

	public:
		// construction / destruction
		constexpr Bounded_Vector() noexcept = default;

		~Bounded_Vector() noexcept {Clear();}

		// copy constructor
		Bounded_Vector(const Bounded_Vector& other)
		{
			for (std::size_t i = 0; i < other.m_size; ++i)
				new (&buffer_[i]) T(*other.Ptr_At(i));
			m_size = other.m_size;
		}
		// move constructor
		Bounded_Vector(Bounded_Vector&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
		{
			for (std::size_t i = 0; i < other.m_size; ++i) {
				new (&buffer_[i]) T(std::move(*other.Ptr_At(i)));
				other.Ptr_At(i)->~T();
			}
			m_size = other.m_size;
			other.m_size = 0;
		}

		// copy assign
		Bounded_Vector& operator=(const Bounded_Vector& other)
		{
			if (this == &other) return *this;
			Clear();
			for (std::size_t i = 0; i < other.m_size; ++i)
				new (&buffer_[i]) T(*other.Ptr_At(i));
			m_size = other.m_size;
			return *this;
		}
		// move assign
		Bounded_Vector& operator=(Bounded_Vector&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
		{
			if (this == &other) return *this;
			Clear();
			for (std::size_t i = 0; i < other.m_size; ++i)
			{
				new (&buffer_[i]) T(std::move(*other.Ptr_At(i)));
				other.Ptr_At(i)->~T();
			}
			m_size = other.m_size;
			other.m_size = 0;
			return *this;
		}

		// Capacity / state
		constexpr std::size_t Capacity() const noexcept { return N; }
		std::size_t Size() const noexcept { return m_size; }
		bool Empty() const noexcept { return m_size == 0; }
		bool Full() const noexcept { return m_size == N; }

		// element access
		T& operator[](std::size_t i) noexcept { return *Ptr_At(i); }
		const T& operator[](std::size_t i) const noexcept { return *Ptr_At(i); }

		T& At(std::size_t i)
		{
			if (i >= m_size) throw Exception{Exception::Type::OUT_OF_RANGE};
			return *Ptr_At(i);
		}
		const T& At(std::size_t i) const
		{
			if(i >= m_size)  throw Exception{Exception::Type::OUT_OF_RANGE};
			return *Ptr_At(i);
		}

		T* Data() noexcept { return Ptr_At(0); }
		const T* Data() const noexcept { return Ptr_At(0); }

		// modifiers
		template<class... Args>
		void Emplace_Back(Args&&... args)
		{
			if(Full()) throw  Exception{Exception::Type::NO_MORE_PLACE_TO_EMPLACE};
			new (&buffer_[m_size]) T(std::forward<Args>(args)...);
			++m_size;
		}

		void Push_Back(const T& v) { Emplace_Back(v); }
		void Push_Back(T&& v) { Emplace_Back(std::move(v)); }

		void Pop_Back()
		{
			if(Empty()) throw Exception{Exception::Type::NO_MORE_ELEMENT_TO_POP};
			--m_size;
			Ptr_At(m_size)->~T();
		}

		void Clear() noexcept
		{
			while (m_size > 0) {
				--m_size;
				Ptr_At(m_size)->~T();
			}
		}

		// iterators
		T* begin() noexcept { return Data(); }
		T* end() noexcept { return Data() + m_size; }
		const T* begin() const noexcept { return Data(); }
		const T* end() const noexcept { return Data() + m_size; }
		const T* cbegin() const noexcept { return Data(); }
		const T* cend() const noexcept { return Data() + m_size; }
	};
}

#endif //UTILS_BOUNDED_ARRAY_H