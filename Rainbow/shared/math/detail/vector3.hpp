#pragma once

#include "../../assert.hpp"
#include "../vector3.hpp"

namespace rainbow {

	template <typename T>
	vector_t<3, T>::vector_t(const T& scalar) : _data{ scalar, scalar, scalar } {}

	template <typename T>
	vector_t<3, T>::vector_t(const T& x, const T& y, const T& z) : _data{ x, y, z } {}

	template <typename T>
	size_t vector_t<3, T>::length() const noexcept
	{
		return 3;
	}

	template <typename T>
	T& vector_t<3, T>::operator[](const size_t& index)
	{
		assert(index < 3);

		return _data[index];
	}

	template <typename T>
	const T& vector_t<3, T>::operator[](const size_t& index) const
	{
		assert(index < 3);

		return _data[index];
	}

	template <typename T>
	typename vector_t<3, T>::type& vector_t<3, T>::operator+=(const T& value) noexcept
	{
		_data[0] += value;
		_data[1] += value;
		_data[2] += value;

		return *this;
	}

	template <typename T>
	typename vector_t<3, T>::type& vector_t<3, T>::operator+=(const vector_t<3, T>& value) noexcept
	{
		_data[0] += value._data[0];
		_data[1] += value._data[1];
		_data[2] += value._data[2];

		return *this;
	}

	template <typename T>
	typename vector_t<3, T>::type vector_t<3, T>::operator+(const T& value) const noexcept
	{
		return vector_t<3, T>(
			_data[0] + value,
			_data[1] + value,
			_data[2] + value);
	}

	template <typename T>
	typename vector_t<3, T>::type vector_t<3, T>::operator+(const vector_t<3, T>& value) const noexcept
	{
		return vector_t<3, T>(
			_data[0] + value._data[0],
			_data[1] + value._data[1],
			_data[2] + value._data[2]);
	}

	template <typename T>
	typename vector_t<3, T>::type& vector_t<3, T>::operator-=(const T& value) noexcept
	{
		_data[0] -= value;
		_data[1] -= value;
		_data[2] -= value;

		return *this;
	}

	template <typename T>
	typename vector_t<3, T>::type& vector_t<3, T>::operator-=(const vector_t<3, T>& value) noexcept
	{
		_data[0] -= value._data[0];
		_data[1] -= value._data[1];
		_data[2] -= value._data[2];

		return *this;
	}

	template <typename T>
	typename vector_t<3, T>::type vector_t<3, T>::operator-(const T& value) const noexcept
	{
		return vector_t<3, T>(
			_data[0] - value,
			_data[1] - value,
			_data[2] - value);
	}

	template <typename T>
	typename vector_t<3, T>::type vector_t<3, T>::operator-(const vector_t<3, T>& value) const noexcept
	{
		return vector_t<3, T>(
			_data[0] - value._data[0],
			_data[1] - value._data[1],
			_data[2] - value._data[2]);
	}

	template <typename T>
	typename vector_t<3, T>::type& vector_t<3, T>::operator*=(const T& value) noexcept
	{
		_data[0] *= value;
		_data[1] *= value;
		_data[2] *= value;

		return *this;
	}

	template <typename T>
	typename vector_t<3, T>::type& vector_t<3, T>::operator*=(const vector_t<3, T>& value) noexcept
	{
		_data[0] *= value._data[0];
		_data[1] *= value._data[1];
		_data[2] *= value._data[2];

		return *this;
	}

	template <typename T>
	typename vector_t<3, T>::type vector_t<3, T>::operator*(const T& value) const noexcept
	{
		return vector_t<3, T>(
			_data[0] * value,
			_data[1] * value,
			_data[2] * value);
	}

	template <typename T>
	typename vector_t<3, T>::type vector_t<3, T>::operator*(const vector_t<3, T>& value) const noexcept
	{
		return vector_t<3, T>(
			_data[0] * value._data[0],
			_data[1] * value._data[1],
			_data[2] * value._data[2]);
	}

	template <typename T>
	typename vector_t<3, T>::type& vector_t<3, T>::operator/=(const T& value)
	{
		assert(value != 0);

		_data[0] /= value;
		_data[1] /= value;
		_data[2] /= value;

		return *this;
	}

	template <typename T>
	typename vector_t<3, T>::type vector_t<3, T>::operator/(const T& value) const
	{
		assert(value != 0);

		return vector_t<3, T>(
			_data[0] / value,
			_data[1] / value,
			_data[2] / value);
	}


}
