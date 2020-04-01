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
			return glm::rotate<T>(identity<T>(), angle, axis);
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

#else

#endif
		
	}
}
