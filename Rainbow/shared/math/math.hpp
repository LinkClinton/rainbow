#pragma once

#include "../utilities.hpp"

#include "bound.hpp"

#include "vector1.hpp"
#include "vector2.hpp"
#include "vector3.hpp"
#include "vector4.hpp"

#include "matrix4x4.hpp"

namespace rainbow {

	namespace math {
		
		using vector1 = vector1_t<real>;
		using vector2 = vector2_t<real>;
		using vector3 = vector3_t<real>;
		using vector4 = vector4_t<real>;

		using vector1i = vector1_t<int32>;
		using vector2i = vector2_t<int32>;
		using vector3i = vector3_t<int32>;
		using vector4i = vector4_t<int32>;

		using matrix4x4 = matrix4x4_t<real>;

		using bound1 = bound1_t<real>;
		using bound2 = bound2_t<real>;
		using bound3 = bound3_t<real>;

		using bound1i = bound1_t<int32>;
		using bound2i = bound2_t<int32>;
		using bound3i = bound2_t<int32>;

		template <typename T>
		T tan(const T value);

		template <typename T>
		T clamp(const T& value, const T& min_limit, const T& max_limit);

		template <typename T>
		T max(const T& v0, const T& v1);

		template <typename T>
		T min(const T& v0, const T& v1);

		template <typename T>
		T sqrt(const T& v);

		template <typename T>
		T abs(const T& v);
	}
}

#include "detail/math.hpp"