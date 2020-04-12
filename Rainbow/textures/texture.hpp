#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../shared/math/math.hpp"

#include <vector>

namespace rainbow {

	using namespace math;
	
	namespace textures {

		template <size_t Dimension>
		using texture_size = vector_t<Dimension, size_t>;
		
		template <size_t Dimension, typename T>
		class texture_t : public interfaces::noncopyable {
		public:
			explicit texture_t(const vector_t<Dimension, size_t>& size);

			~texture_t() = default;

			vector_t<Dimension, size_t> size() const noexcept;

			virtual T sample(const vector_t<Dimension, real>& position) = 0;
		protected:
			vector_t<Dimension, size_t> mSize;
		};
		
	}
}

#include "detail/texture.hpp"