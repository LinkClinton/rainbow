#pragma once

#include "vector4.hpp"

namespace rainbow {

	namespace math {

#ifndef __GLM_IMPLEMENTATION__



#endif

		template <typename T>
		using matrix4x4_t = glm::mat<4, 4, T>;

		template <typename T>
		matrix4x4_t<T> identity();

		template <typename T>
		matrix4x4_t<T> translate(const vector3_t<T>& vec);

		template <typename T>
		matrix4x4_t<T> rotate(const T angle, const vector3_t<T>& axis);

		template <typename T>
		matrix4x4_t<T> scale(const vector3_t<T>& vec);

		template <typename T>
		matrix4x4_t<T> transpose(const matrix4x4_t<T>& matrix);

		template <typename T>
		matrix4x4_t<T> inverse(const matrix4x4_t<T>& matrix);

		template <typename T>
		matrix4x4_t<T> perspective(real fov, real width, real height, real near, real far);
	}
}

#include "../math/detail/matrix4x4.hpp"
