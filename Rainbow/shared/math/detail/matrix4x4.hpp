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
		matrix4x4_t<T> perspective(const real fov, const real near, const real far)
		{
			const auto perspective = matrix4x4_t<T>(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, far / (far - near), -(far * near) / (far - near),
				0, 0, 1, 0
			);

			const auto invTanAngle = 1 / glm::tan(fov / 2);

			return scale(vector3_t<T>(invTanAngle, invTanAngle, 1)) * perspective;
		}

#else

#endif
		
	}
}
