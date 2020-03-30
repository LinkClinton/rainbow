#pragma once

#include "vector4.hpp"

namespace rainbow {

	template<typename T>
	struct matrix4x4_t {
	public:
		using column_type = vector4_t<T>;

		matrix4x4_t() = default;
		
		matrix4x4_t(
			const column_type& v0,
			const column_type& v1,
			const column_type& v2,
			const column_type& v3);

		matrix4x4_t(
			const T& x0, const T& y0, const T& z0, const T& w0,
			const T& x1, const T& y1, const T& z1, const T& w1,
			const T& x2, const T& y2, const T& z2, const T& w2,
			const T& x3, const T& y3, const T& z3, const T& w3);
	private:
		column_type data[4];
	};

}

#include "../math/detail/matrix4x4.hpp"
