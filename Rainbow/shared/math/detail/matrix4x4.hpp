#pragma once

#include "../matrix4x4.hpp"

namespace rainbow {

	namespace math {

#ifdef __GLM_IMPLEMENTATION__
		
		template<typename T>
		matrix4x4_t<T> identity()
		{
			return matrix4x4_t<T>(1);
		}
		
#else

#endif
		
	}
}
