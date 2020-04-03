#pragma once

#include "../math.hpp"

namespace rainbow {

	namespace math {

#ifdef __GLM_IMPLEMENTATION__

		
		template <typename T>
		T tan(const T value) {
			return glm::tan(value);
		}

		template <typename T>
		T clamp(const T& value, const T& min_limit, const T& max_limit)
		{
			return glm::clamp(value, min_limit, max_limit);
		}

		template <typename T>
		T max(const T& v0, const T& v1)
		{
			return glm::max(v0, v1);
		}

		template <typename T>
		T min(const T& v0, const T& v1)
		{
			return glm::min(v0, v1);
		}

		template <typename T>
		T sqrt(const T& v)
		{
			return glm::sqrt(v);
		}

		template <typename T>
		T abs(const T& v)
		{
			return glm::abs(v);
		}


#endif
	}
}
