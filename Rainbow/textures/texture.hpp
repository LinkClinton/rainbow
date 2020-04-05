#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../shared/math/math.hpp"

#include <vector>

namespace rainbow {

	using namespace math;
	
	namespace textures {

		template <size_t Dimension, typename T>
		class texture_t;

		template <size_t Dimension, typename T>
		class texture_t<2, T> : public interfaces::noncopyable {
		public:
			explicit texture_t(size_t width, size_t height);

			virtual T sample(const vector2& uv) const = 0;
			
			size_t width() const noexcept;

			size_t height() const noexcept;
		protected:
			size_t mWidth, mHeight;
		};
		
		template <typename T>
		using texture2d = texture_t<2, T>;
	}
}

#include "detail/texture.hpp"