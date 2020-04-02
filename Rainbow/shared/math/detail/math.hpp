#pragma once

#include "../math.hpp"

namespace rainbow {

	namespace math {

#ifdef __GLM_IMPLEMENTATION__

		
		template <typename T>
		T tan(const T value) {
			return glm::tan(value);
		}

		
#endif
	}
}
