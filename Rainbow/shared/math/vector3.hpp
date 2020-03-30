#pragma once

#include "vector.hpp"

namespace rainbow {

	template <typename T>
	struct vector_t<3, T> {
		using type = vector_t<3, T>;

		union {
			struct { T x, y, z; };
			struct { T _data[3]; };
		};
		
		vector_t() = default;

		vector_t(const T& scalar);
		
		vector_t(const T& x, const T& y, const T& z);

		size_t length() const noexcept;

		T& operator[](const size_t& index);
		const T& operator[](const size_t& index) const;

		type& operator+=(const T& value) noexcept;
		type& operator+=(const vector_t<3, T>& value) noexcept;
		type operator+(const T& value) const noexcept;
		type operator+(const vector_t<3, T>& value) const noexcept;

		type& operator-=(const T& value) noexcept;
		type& operator-=(const vector_t<3, T>& value) noexcept;
		type operator-(const T& value) const noexcept;
		type operator-(const vector_t<3, T>& value) const noexcept;

		type& operator*=(const T& value) noexcept;
		type& operator*=(const vector_t<3, T>& value) noexcept;
		type operator*(const T& value) const noexcept;
		type operator*(const vector_t<3, T>& value) const noexcept;

		type& operator/=(const T& value);
		type operator/(const T& value) const;
	};

	
	template <typename T>
	using vector3_t = vector_t<3, T>;
}

#include "../math/detail/vector3.hpp"