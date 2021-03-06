#pragma once

#include "../utilities.hpp"

#define __GLM_IMPLEMENTATION__
#ifdef  __GLM_IMPLEMENTATION__
#include <glm/glm.hpp>
#endif

namespace rainbow::core::math {

#ifdef __GLM_IMPLEMENTATION__

	template <size_t L, typename T>
	using vector_t = glm::vec<L, T>;

#else

	template <size_t L, typename T>
	struct vector_t;

#endif

	template <size_t L, typename T>
	vector_t<L, T> cross(const vector_t<L, T>& v0, const vector_t<L, T>& v1);

	template <size_t L, typename T>
	vector_t<L, T> normalize(const vector_t<L, T>& v);

	template <size_t L, typename T>
	T dot(const vector_t<L, T>& v0, const vector_t<L, T>& v1);

	template <size_t L, typename T>
	T length(const vector_t<L, T>& v);

	template <size_t L, typename T>
	T distance(const vector_t<L, T>& v0, const vector_t<L, T>& v1);

	template <size_t L, typename T>
	T distance_squared(const vector_t<L, T>& v0, const vector_t<L, T>& v1);

	template <size_t L, typename T>
	T length_squared(const vector_t<L, T>& v);

	template <size_t L, typename T>
	T max_component(const vector_t<L, T>& v);

	template <size_t L, typename T>
	bool has_nan(const vector_t<L, T>& v);

	template <size_t L, typename T>
	vector_t<L, T> abs(const vector_t<L, T>& v);

	template <size_t L, typename T>
	vector_t<L, T> min(const vector_t<L, T>& v0, const vector_t<L, T>& v1);

	template <size_t L, typename T>
	vector_t<L, T> max(const vector_t<L, T>& v0, const vector_t<L, T>& v1);

	template <size_t L, typename T>
	vector_t<L, T> ceil(const vector_t<L, T>& v);

	template <size_t L, typename T>
	vector_t<L, T> floor(const vector_t<L, T>& v);

	template <size_t L, typename T>
	vector_t<L, T> lerp(const vector_t<L, T>& s, const vector_t<L, T>& t, T x);
}


#include "../math/detail/vector.hpp"