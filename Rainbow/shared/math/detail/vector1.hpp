#pragma once

#include "../../assert.hpp"
#include "../vector1.hpp"

namespace rainbow {
	
	template <typename T>
	vector_t<1, T>::vector_t(const T& x) : _data{ x } {}

	template <typename T>
	size_t vector_t<1, T>::length() const noexcept
	{
		return 1;
	}

	template <typename T>
	T& vector_t<1, T>::operator[](const size_t& index)
	{
		assert(index == 0);

		return _data[index];
	}

	template <typename T>
	const T& vector_t<1, T>::operator[](const size_t& index) const
	{
		assert(index == 0);

		return _data[index];
	}

	template <typename T>
	typename vector_t<1, T>::type& vector_t<1, T>::operator+=(const T& value) noexcept
	{
		_data[0] += value;

		return *this;
	}

	template <typename T>
	typename vector_t<1, T>::type& vector_t<1, T>::operator+=(const vector_t<1, T>& value) noexcept
	{
		_data[0] += value._data[0];

		return *this;
	}

	template <typename T>
	typename vector_t<1, T>::type vector_t<1, T>::operator+(const T& value) const noexcept
	{
		return vector_t<1, T>(_data[0] + value);
	}

	template <typename T>
	typename vector_t<1, T>::type vector_t<1, T>::operator+(const vector_t<1, T>& value) const noexcept
	{
		return vector_t<1, T>(_data[0] + value._data[0]);
	}

	template <typename T>
	typename vector_t<1, T>::type& vector_t<1, T>::operator-=(const T& value) noexcept
	{
		_data[0] -= value;

		return *this;
	}

	template <typename T>
	typename vector_t<1, T>::type& vector_t<1, T>::operator-=(const vector_t<1, T>& value) noexcept
	{
		_data[0] -= value._data[0];

		return *this;
	}

	template <typename T>
	typename vector_t<1, T>::type vector_t<1, T>::operator-(const T& value) const noexcept
	{
		return vector_t<1, T>(_data[0] - value);
	}

	template <typename T>
	typename vector_t<1, T>::type vector_t<1, T>::operator-(const type& value) const noexcept
	{
		return vector_t<1, T>(_data[0] - value._data[0]);
	}

	template <typename T>
	typename vector_t<1, T>::type& vector_t<1, T>::operator*=(const T& value) noexcept
	{
		_data[0] *= value;

		return *this;
	}

	template <typename T>
	typename vector_t<1, T>::type& vector_t<1, T>::operator*=(const vector_t<1, T>& value) noexcept
	{
		_data[0] *= value._data[0];

		return *this;
	}

	template <typename T>
	typename vector_t<1, T>::type vector_t<1, T>::operator*(const T& value) const noexcept
	{
		return vector_t<1, T>(_data[0] * value);
	}

	template <typename T>
	typename vector_t<1, T>::type vector_t<1, T>::operator*(const vector_t<1, T>& value) const noexcept
	{
		return vector_t<1, T>(_data[0] * value._data[0]);
	}

	template <typename T>
	typename vector_t<1, T>::type& vector_t<1, T>::operator/=(const T& value)
	{
		assert(value != 0);
		
		_data[0] /= value;

		return *this;
	}

	template <typename T>
	typename vector_t<1, T>::type vector_t<1, T>::operator/(const T& value) const
	{
		assert(value != 0);

		return vector_t<1, T>(_data[0] / value);
	}

}
