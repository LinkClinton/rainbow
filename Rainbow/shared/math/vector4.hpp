#pragma once

#include "vector.hpp"

namespace rainbow {

	template <typename T>
	struct vector_t<4, T> {
		using type = vector_t<4, T>;

		union {
			struct { T x, y, z, w; };
			struct { T _data[4]; };
		};
		
		vector_t() = default;

		vector_t(const T& scalar);
		
		vector_t(const T& x, const T& y, const T& z, const T& w);

		size_t length() const noexcept;

		T& operator[](const size_t& index);
		const T& operator[](const size_t& index) const;

		type& operator+=(const T& value) noexcept;
		type& operator+=(const vector_t<4, T>& value) noexcept;
		type operator+(const T& value) const noexcept;
		type operator+(const vector_t<4, T>& value) const noexcept;

		type& operator-=(const T& value) noexcept;
		type& operator-=(const vector_t<4, T>& value) noexcept;
		type operator-(const T& value) const noexcept;
		type operator-(const vector_t<4, T>& value) const noexcept;

		type& operator*=(const T& value) noexcept;
		type& operator*=(const vector_t<4, T>& value) noexcept;
		type operator*(const T& value) const noexcept;
		type operator*(const vector_t<4, T>& value) const noexcept;

		type& operator/=(const T& value);
		type operator/(const T& value) const;
	};

	template <typename T>
	using vector4_t = vector_t<4, T>;
}
