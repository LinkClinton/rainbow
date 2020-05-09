#pragma once

#include "../math/vector.hpp"

namespace rainbow {

	namespace math {

#ifndef __GLM_IMPLEMENTATION__



#endif

		template <typename T>
		using vector1_t = vector_t<1, T>;
		
	}
}

#include "../math/detail/vector1.hpp"