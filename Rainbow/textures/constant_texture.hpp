#pragma once

#include "texture.hpp"

namespace rainbow {

	namespace textures {

		template <size_t Dimension, typename T>
		class constant_texture_t final : public texture_t<Dimension, T> {
		public:
			explicit constant_texture_t(const T& value, const vector_t<Dimension, size_t>& size = texture_size<Dimension>(1));

			T sample(const vector_t<Dimension, real>& position) override;
		private:
			T mValue;
		};

		template <typename T>
		using constant_texture2d = constant_texture_t<2, T>;
		
	}
}

#include "detail/constant_texture.hpp"