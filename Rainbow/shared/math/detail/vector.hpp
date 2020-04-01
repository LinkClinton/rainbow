#pragma once

#include "../vector.hpp"

namespace rainbow {

	namespace math {

#ifdef __GLM_IMPLEMENTATION__

		template <size_t L, typename T>
		vector_t<L, T> cross(const vector_t<L, T>& v0, const vector_t<L, T>& v1)
		{
			return glm::cross(v0, v1);
		}

		template <size_t L, typename T>
		vector_t<L, T> normalize(const vector_t<L, T>& v)
		{
			return glm::normalize(v);
		}

		template <size_t L, typename T>
		T dot(const vector_t<L, T>& v0, const vector_t<L, T>& v1)
		{
			return glm::dot(v0, v1);
		}

		template <size_t L, typename T>
		T length(const vector_t<L, T>& v)
		{
			return glm::length(v);
		}

		template <size_t L, typename T>
		vector_t<L, T> min(const vector_t<L, T>& v0, const vector_t<L, T>& v1)
		{
			return glm::min(v0, v1);
		}

		template <size_t L, typename T>
		vector_t<L, T> max(const vector_t<L, T>& v0, const vector_t<L, T>& v1)
		{
			return glm::max(v0, v1);
		}

#else
		
#endif
		
	}
}
