#pragma once

#include "../../assert.hpp"
#include "../vector2.hpp"

namespace rainbow {

	template <typename T>
	vector_t<2, T>::vector_t(const T& scalar) : _data{ scalar, scalar } {}

	template <typename T>
	vector_t<2, T>::vector_t(const T& x, const T& y) : _data{ x, y } {}

	template <typename T>
	size_t vector_t<2, T>::length() const noexcept
	{
		return 2;
	}

	template <typename T>
	T& vector_t<2, T>::operator[](const size_t& index)
	{
		assert(index < 2);

		return _data[index];
	}

	template <typename T>
	const T& vector_t<2, T>::operator[](const size_t& index) const
	{
		assert(index < 2);

		return _data[index];
	}

	template <typename T>
	typename vector_t<2, T>::type& vector_t<2, T>::operator+=(const T& value) noexcept
	{
		_data[0] += value;
		_data[1] += value;

		return *this;
	}

	template <typename T>
	typename vector_t<2, T>::type& vector_t<2, T>::operator+=(const vector_t<2, T>& value) noexcept
	{
		_data[0] += value._data[0];
		_data[1] += value._data[1];

		return *this;
	}

	template <typename T>
	typename vector_t<2, T>::type vector_t<2, T>::operator+(const T& value) const noexcept
	{
		return vector_t<2, T>(
			_data[0] + value,
			_data[1] + value);
	}

	template <typename T>
	typename vector_t<2, T>::type vector_t<2, T>::operator+(const vector_t<2, T>& value) const noexcept
	{
		return vector_t<2, T>(
			_data[0] + value._data[0],
			_data[1] + value._data[1]);
	}

	template <typename T>
	typename vector_t<2, T>::type& vector_t<2, T>::operator-=(const T& value) noexcept
	{
		_data[0] -= value;
		_data[1] -= value;

		return *this;
	}

	template <typename T>
	typename vector_t<2, T>::type& vector_t<2, T>::operator-=(const vector_t<2, T>& value) noexcept
	{
		_data[0] -= value._data[0];
		_data[1] -= value._data[1];

		return *this;
	}

	template <typename T>
	typename vector_t<2, T>::type vector_t<2, T>::operator-(const T& value) const noexcept
	{
		return vector_t<2, T>(
			_data[0] - value,
			_data[1] - value);
	}

	template <typename T>
	typename vector_t<2, T>::type vector_t<2, T>::operator-(const vector_t<2, T>& value) const noexcept
	{
		return vector_t<2, T>(
			_data[0] - value._data[0],
			_data[1] - value._data[1]);
	}

	template <typename T>
	typename vector_t<2, T>::type& vector_t<2, T>::operator*=(const T& value) noexcept
	{
		_data[0] *= value;
		_data[1] *= value;

		return *this;
	}

	template <typename T>
	typename vector_t<2, T>::type& vector_t<2, T>::operator*=(const vector_t<2, T>& value) noexcept
	{
		_data[0] *= value._data[0];
		_data[1] *= value._data[1];

		return *this;
	}

	template <typename T>
	typename vector_t<2, T>::type vector_t<2, T>::operator*(const T& value) const noexcept
	{
		return vector_t<2, T>(
			_data[0] * value,
			_data[1] * value);
	}

	template <typename T>
	typename vector_t<2, T>::type vector_t<2, T>::operator*(const vector_t<2, T>& value) const noexcept
	{
		return vector_t<2, T>(
			_data[0] * value._data[0],
			_data[1] * value._data[1]);
	}

	template <typename T>
	typename vector_t<2, T>::type& vector_t<2, T>::operator/=(const T& value)
	{
		assert(value != 0);

		_data[0] /= value;
		_data[1] /= value;

		return *this;
	}

	template <typename T>
	typename vector_t<2, T>::type vector_t<2, T>::operator/(const T& value) const
	{
		assert(value != 0);

		return vector_t<2, T>(
			_data[0] / value,
			_data[1] / value);
	}


}
