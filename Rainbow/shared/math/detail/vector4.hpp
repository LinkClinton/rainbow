#pragma once

#include "../../assert.hpp"
#include "../vector4.hpp"

namespace rainbow {

	template <typename T>
	vector_t<4, T>::vector_t(const T& scalar) : _data{ scalar, scalar, scalar, scalar } {}

	template <typename T>
	vector_t<4, T>::vector_t(const T& x, const T& y, const T& z, const T& w) : _data{ x, y, z, w } {}

	template <typename T>
	size_t vector_t<4, T>::length() const noexcept
	{
		return 4;
	}

	template <typename T>
	T& vector_t<4, T>::operator[](const size_t& index)
	{
		assert(index < 4);

		return _data[index];
	}

	template <typename T>
	const T& vector_t<4, T>::operator[](const size_t& index) const
	{
		assert(index < 4);

		return _data[index];
	}

	template <typename T>
	typename vector_t<4, T>::type& vector_t<4, T>::operator+=(const T& value) noexcept
	{
		_data[0] += value;
		_data[1] += value;
		_data[2] += value;
		_data[3] += value;

		return *this;
	}

	template <typename T>
	typename vector_t<4, T>::type& vector_t<4, T>::operator+=(const vector_t<4, T>& value) noexcept
	{
		_data[0] += value._data[0];
		_data[1] += value._data[1];
		_data[2] += value._data[2];
		_data[3] += value._data[3];

		return *this;
	}

	template <typename T>
	typename vector_t<4, T>::type vector_t<4, T>::operator+(const T& value) const noexcept
	{
		return vector_t<4, T>(
			_data[0] + value,
			_data[1] + value,
			_data[2] + value,
			_data[3] + value);
	}

	template <typename T>
	typename vector_t<4, T>::type vector_t<4, T>::operator+(const vector_t<4, T>& value) const noexcept
	{
		return vector_t<4, T>(
			_data[0] + value._data[0],
			_data[1] + value._data[1],
			_data[2] + value._data[2],
			_data[3] + value._data[3]);
	}

	template <typename T>
	typename vector_t<4, T>::type& vector_t<4, T>::operator-=(const T& value) noexcept
	{
		_data[0] -= value;
		_data[1] -= value;
		_data[2] -= value;
		_data[3] -= value;

		return *this;
	}

	template <typename T>
	typename vector_t<4, T>::type& vector_t<4, T>::operator-=(const vector_t<4, T>& value) noexcept
	{
		_data[0] -= value._data[0];
		_data[1] -= value._data[1];
		_data[2] -= value._data[2];
		_data[3] -= value._data[3];

		return *this;
	}

	template <typename T>
	typename vector_t<4, T>::type vector_t<4, T>::operator-(const T& value) const noexcept
	{
		return vector_t<4, T>(
			_data[0] - value,
			_data[1] - value,
			_data[2] - value,
			_data[3] - value);
	}

	template <typename T>
	typename vector_t<4, T>::type vector_t<4, T>::operator-(const vector_t<4, T>& value) const noexcept
	{
		return vector_t<4, T>(
			_data[0] - value._data[0],
			_data[1] - value._data[1],
			_data[2] - value._data[2],
			_data[3] - value._data[3]);
	}

	template <typename T>
	typename vector_t<4, T>::type& vector_t<4, T>::operator*=(const T& value) noexcept
	{
		_data[0] *= value;
		_data[1] *= value;
		_data[2] *= value;
		_data[3] *= value;

		return *this;
	}

	template <typename T>
	typename vector_t<4, T>::type& vector_t<4, T>::operator*=(const vector_t<4, T>& value) noexcept
	{
		_data[0] *= value._data[0];
		_data[1] *= value._data[1];
		_data[2] *= value._data[2];
		_data[3] *= value._data[3];

		return *this;
	}

	template <typename T>
	typename vector_t<4, T>::type vector_t<4, T>::operator*(const T& value) const noexcept
	{
		return vector_t<4, T>(
			_data[0] * value,
			_data[1] * value,
			_data[2] * value,
			_data[3] * value);
	}

	template <typename T>
	typename vector_t<4, T>::type vector_t<4, T>::operator*(const vector_t<4, T>& value) const noexcept
	{
		return vector_t<4, T>(
			_data[0] * value._data[0],
			_data[1] * value._data[1],
			_data[2] * value._data[2],
			_data[3] * value._data[3]);
	}

	template <typename T>
	typename vector_t<4, T>::type& vector_t<4, T>::operator/=(const T& value)
	{
		assert(value != 0);

		_data[0] /= value;
		_data[1] /= value;
		_data[2] /= value;
		_data[3] /= value;

		return *this;
	}

	template <typename T>
	typename vector_t<4, T>::type vector_t<4, T>::operator/(const T& value) const
	{
		assert(value != 0);

		return vector_t<4, T>(
			_data[0] / value,
			_data[1] / value,
			_data[2] / value,
			_data[3] / value);
	}

}
