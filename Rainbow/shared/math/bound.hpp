#pragma once

#include "vector.hpp"

namespace rainbow {

	namespace math {

		template <size_t Dimension, typename T>
		struct bound_t {
			vector_t<Dimension, T> min;
			vector_t<Dimension, T> max;

			bound_t();

			bound_t(const vector_t<Dimension, T>& p0, const vector_t<Dimension, T>& p1);

			void union_it(const bound_t<Dimension, T>& bound);

			void union_it(const vector_t<Dimension, T>& v);
		};

		template <typename T>
		using bound1_t = bound_t<1, T>;

		template <typename T>
		using bound2_t = bound_t<2, T>;

		template <typename T>
		using bound3_t = bound_t<3, T>;
	}
}

#include "detail/bound.hpp"