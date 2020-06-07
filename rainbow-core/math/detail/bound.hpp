#pragma once

#include "../bound.hpp"

namespace rainbow::core::math {

	template <size_t Dimension, typename T>
	bound_t<Dimension, T>::bound_t() : bound_t(vector_t<Dimension, T>(0), vector_t<Dimension, T>(1))
	{
	}

	template <size_t Dimension, typename T>
	bound_t<Dimension, T>::bound_t(const vector_t<Dimension, T>& p0, const vector_t<Dimension, T>& p1) :
		min(math::min(p0, p1)), max(math::max(p0, p1))
	{
	}

	template <size_t Dimension, typename T>
	void bound_t<Dimension, T>::union_it(const bound_t<Dimension, T>& bound)
	{
		min = math::min(min, bound.min);
		max = math::max(max, bound.max);
	}

	template <size_t Dimension, typename T>
	void bound_t<Dimension, T>::union_it(const vector_t<Dimension, T>& v)
	{
		min = math::min(min, v);
		max = math::max(max, v);
	}

}