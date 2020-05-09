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
		T cos(const T value)
		{
			return glm::cos(value);
		}

		template <typename T>
		T sin(const T value)
		{
			return glm::sin(value);
		}

		template <typename T>
		T atan2(const T y, const T x)
		{
			return glm::atan(y, x);
		}

		template <typename T>
		T acos(const T value)
		{
			return glm::acos(value);
		}

		template <typename T>
		T radians(const T value)
		{
			return glm::radians(value);
		}

		template <typename T>
		T clamp(const T& value, const T& min_limit, const T& max_limit)
		{
			return glm::clamp(value, min_limit, max_limit);
		}

		template <typename T>
		T floor(const T& value)
		{
			return glm::floor(value);
		}

		template <typename T>
		T ceil(const T& value)
		{
			return glm::ceil(value);
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
		T pow(const T& x, const T& y)
		{
			return glm::pow(x, y);
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

		template <typename T>
		T log(const T& x)
		{
			return glm::log(x);
		}

		template <typename T>
		T exp(const T& x)
		{
			return glm::exp(x);
		}

		template <typename T>
		T one_over_two_pi()
		{
			return glm::one_over_two_pi<T>();
		}

		template <typename T>
		T one_over_pi()
		{
			return glm::one_over_pi<T>();
		}

		template <typename T>
		T quarter_pi()
		{
			return glm::quarter_pi<T>();
		}

		template <typename T>
		T half_pi()
		{
			return glm::half_pi<T>();
		}

		template <typename T>
		T two_pi()
		{
			return glm::two_pi<T>();
		}

		template <typename T>
		T pi()
		{
			return glm::pi<T>();
		}


#endif
	}
}
