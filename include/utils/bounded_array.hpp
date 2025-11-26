#ifndef UTILS_BOUNDED_ARRAY_HPP
#define UTILS_BOUNDED_ARRAY_HPP

#include "bounded_array.h"
#include <utility>
#include <cassert>


// helper implementations
template<typename T, std::size_t N>
T* O::Bounded_Vector<T, N>::Ptr_At(std::size_t i) noexcept
{
	return reinterpret_cast<T*>(&m_buffer[i]);
}

template<typename T, std::size_t N>
const T* O::Bounded_Vector<T, N>::Ptr_At(std::size_t i) const noexcept
{
	return reinterpret_cast<const T*>(&m_buffer[i]);
}

// destructor
template<typename T, std::size_t N>
O::Bounded_Vector<T, N>::~Bounded_Vector() noexcept
{
	Clear();
}

// copy ctor
template<typename T, std::size_t N>
O::Bounded_Vector<T, N>::Bounded_Vector(const Bounded_Vector& other)
{
	for (std::size_t i = 0; i < other.m_size; ++i)
		new (&m_buffer[i]) T(*other.Ptr_At(i));
	m_size = other.m_size;
}

// move ctor
template<typename T, std::size_t N>
O::Bounded_Vector<T, N>::Bounded_Vector(Bounded_Vector&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
{
	for (std::size_t i = 0; i < other.m_size; ++i) {
		new (&m_buffer[i]) T(std::move(*other.Ptr_At(i)));
		other.Ptr_At(i)->~T();
	}
	m_size = other.m_size;
	other.m_size = 0;
}

// copy assign
template<typename T, std::size_t N>
O::Bounded_Vector<T, N>& O::Bounded_Vector<T, N>::operator=(const Bounded_Vector& other)
{
	if (this == &other) return *this;
	Clear();
	for (std::size_t i = 0; i < other.m_size; ++i)
		new (&m_buffer[i]) T(*other.Ptr_At(i));
	m_size = other.m_size;
	return *this;
}

// move assign
template<typename T, std::size_t N>
O::Bounded_Vector<T, N>& O::Bounded_Vector<T, N>::operator=(Bounded_Vector&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
{
	if (this == &other) return *this;
	Clear();
	for (std::size_t i = 0; i < other.m_size; ++i)
	{
		new (&m_buffer[i]) T(std::move(*other.Ptr_At(i)));
		other.Ptr_At(i)->~T();
	}
	m_size = other.m_size;
	other.m_size = 0;
	return *this;
}

// Capacity / state
template<typename T, std::size_t N>
constexpr std::size_t O::Bounded_Vector<T, N>::Capacity() const noexcept { return N; }

template<typename T, std::size_t N>
std::size_t O::Bounded_Vector<T, N>::Size() const noexcept { return m_size; }

template<typename T, std::size_t N>
bool O::Bounded_Vector<T, N>::Empty() const noexcept { return m_size == 0; }

template<typename T, std::size_t N>
bool O::Bounded_Vector<T, N>::Full() const noexcept { return m_size == N; }

// element access
template<typename T, std::size_t N>
T& O::Bounded_Vector<T, N>::operator[](std::size_t i) noexcept { return *Ptr_At(i); }

template<typename T, std::size_t N>
const T& O::Bounded_Vector<T, N>::operator[](std::size_t i) const noexcept { return *Ptr_At(i); }

template<typename T, std::size_t N>
T& O::Bounded_Vector<T, N>::At(std::size_t i)
{
	if (i >= m_size) throw Exception{Exception::Type::OUT_OF_RANGE};
	return *Ptr_At(i);
}

template<typename T, std::size_t N>
const T& O::Bounded_Vector<T, N>::At(std::size_t i) const
{
	if(i >= m_size)  throw Exception{Exception::Type::OUT_OF_RANGE};
	return *Ptr_At(i);
}

template<typename T, std::size_t N>
T* O::Bounded_Vector<T, N>::Data() noexcept { return Ptr_At(0); }

template<typename T, std::size_t N>
const T* O::Bounded_Vector<T, N>::Data() const noexcept { return Ptr_At(0); }

// modifiers
template<typename T, std::size_t N>
template<class... Args>
void O::Bounded_Vector<T, N>::Emplace_Back(Args&&... args)
{
	if(Full()) throw  Exception{Exception::Type::NO_MORE_PLACE_TO_EMPLACE};
	new (&m_buffer[m_size]) T(std::forward<Args>(args)...);
	++m_size;
}

template<typename T, std::size_t N>
void O::Bounded_Vector<T, N>::Push_Back(const T& v) { Emplace_Back(v); }

template<typename T, std::size_t N>
void O::Bounded_Vector<T, N>::Push_Back(T&& v) { Emplace_Back(std::move(v)); }

template<typename T, std::size_t N>
void O::Bounded_Vector<T, N>::Pop_Back()
{
	if(Empty()) throw Exception{Exception::Type::NO_MORE_ELEMENT_TO_POP};
	--m_size;
	Ptr_At(m_size)->~T();
}

template<typename T, std::size_t N>
void O::Bounded_Vector<T, N>::Clear() noexcept
{
	while (m_size > 0) {
		--m_size;
		Ptr_At(m_size)->~T();
	}
}

// iterators
template<typename T, std::size_t N>
T* O::Bounded_Vector<T, N>::begin() noexcept { return Data(); }

template<typename T, std::size_t N>
T* O::Bounded_Vector<T, N>::end() noexcept { return Data() + m_size; }

template<typename T, std::size_t N>
const T* O::Bounded_Vector<T, N>::begin() const noexcept { return Data(); }

template<typename T, std::size_t N>
const T* O::Bounded_Vector<T, N>::end() const noexcept { return Data() + m_size; }

template<typename T, std::size_t N>
const T* O::Bounded_Vector<T, N>::cbegin() const noexcept { return Data(); }

template<typename T, std::size_t N>
const T* O::Bounded_Vector<T, N>::cend() const noexcept { return Data() + m_size; }

#endif // UTILS_BOUNDED_ARRAY_HPP
