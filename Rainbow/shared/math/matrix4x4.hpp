#pragma once

#include "vector4.hpp"

namespace rainbow {

	namespace math {

#ifndef __GLM_IMPLEMENTATION__



#endif

		template<typename T>
		using matrix4x4_t = glm::mat<4, 4, T>;

		template<typename T>
		matrix4x4_t<T> identity();
	}
}

#include "../math/detail/matrix4x4.hpp"
