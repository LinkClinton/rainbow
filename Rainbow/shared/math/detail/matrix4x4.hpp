#pragma once

#include "../matrix4x4.hpp"

namespace rainbow {

	template <typename T>
	matrix4x4_t<T>::matrix4x4_t(
		const column_type& v0,
		const column_type& v1,
		const column_type& v2,
		const column_type& v3)
	{
		data = { v0, v1, v2, v3 };
	}

	template <typename T>
	matrix4x4_t<T>::matrix4x4_t(
		const T& x0, const T& y0, const T& z0, const T& w0,
		const T& x1, const T& y1, const T& z1, const T& w1,
		const T& x2, const T& y2, const T& z2, const T& w2,
		const T& x3, const T& y3, const T& z3, const T& w3)
	{
		data = {
			column_type(x0, y0, z0, w0),
			column_type(x1, y1, z1, w1),
			column_type(x2, y2, z2, w2),
			column_type(x3, y3, z3, w3)
		};
	}
	
}
