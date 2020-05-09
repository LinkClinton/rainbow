#pragma once

#include "vector.hpp"

namespace rainbow {

	namespace math {

#ifndef __GLM_IMPLEMENTATION__



#endif

		template <typename T>
		using vector2_t = vector_t<2, T>;
		
	}
}

#include "../math/detail/vector2.hpp"