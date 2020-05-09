#pragma once

#include "../matrix4x4.hpp"

#ifdef __GLM_IMPLEMENTATION__
#include <glm/gtc/matrix_transform.hpp>
#endif

namespace rainbow {

	namespace math {

#ifdef __GLM_IMPLEMENTATION__
		
		template<typename T>
		matrix4x4_t<T> identity()
		{
			return matrix4x4_t<T>(1);
		}

		template <typename T>
		matrix4x4_t<T> translate(const vector3_t<T>& vec)
		{
			return glm::translate<T>(identity<T>(), vec);
		}

		template <typename T>
		matrix4x4_t<T> rotate(const T angle, const vector3_t<T>& axis)
		{
			return glm::rotate<T>(identity<T>(), glm::radians(angle), axis);
		}

		template <typename T>
		matrix4x4_t<T> scale(const vector3_t<T>& vec)
		{
			return glm::scale<T>(identity<T>(), vec);
		}

		template <typename T>
		matrix4x4_t<T> transpose(const matrix4x4_t<T>& matrix)
		{
			return glm::transpose(matrix);
		}

		template <typename T>
		matrix4x4_t<T> inverse(const matrix4x4_t<T>& matrix)
		{
			return glm::inverse(matrix);
		}

		template <typename T>
		matrix4x4_t<T> perspective(real fov, real width, real height, real near, real far)
		{
			return glm::perspectiveFov(fov, width, height, near, far);
		}

		template <typename T>
		matrix4x4_t<T> look_at_right_hand(const vector3_t<T>& origin, const vector3_t<T>& target, const vector3_t<T>& up)
		{
			return glm::lookAtRH(origin, target, up);
		}

		template <typename T>
		matrix4x4_t<T> look_at_left_hand(const vector3_t<T>& origin, const vector3_t<T>& target, const vector3_t<T>& up)
		{
			return glm::lookAtLH(origin, target, up);
		}

#else

#endif
		
	}
}
