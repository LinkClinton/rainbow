#pragma once

#include "../math/vector.hpp"

namespace rainbow {

	template <typename T>
	struct vector_t<1, T> {
		
		using type = vector_t<1, T>;

		union {
			struct { T x; };
			struct { T _data[1]; };
		};
		
		vector_t() = default;

		vector_t(const T& x);
		
		size_t length() const noexcept;
		
		T& operator[](const size_t& index);
		const T& operator[](const size_t& index) const;

		type& operator+=(const T& value) noexcept;
		type& operator+=(const vector_t<1, T>& value) noexcept;
		type operator+(const T& value) const noexcept;
		type operator+(const vector_t<1, T>& value) const noexcept;

		type& operator-=(const T& value) noexcept;
		type& operator-=(const vector_t<1, T>& value) noexcept;
		type operator-(const T& value) const noexcept;
		type operator-(const type& value) const noexcept;
	
		type& operator*=(const T& value) noexcept;
		type& operator*=(const vector_t<1, T>& value) noexcept;
		type operator*(const T& value) const noexcept;
		type operator*(const vector_t<1, T>& value) const noexcept;

		type& operator/=(const T& value);
		type operator/(const T& value) const;
	};

	template <typename T>
	using vector1_t = vector_t<1, T>;
}

#include "../math/detail/vector1.hpp"