#pragma once

#include "texture.hpp"

namespace rainbow {

	namespace textures {

		template <size_t Dimension, typename T>
		class mixture_texture final : public texture_t<Dimension, T> {
		public:
			explicit mixture_texture(
				const std::shared_ptr<texture_t<Dimension, T>>& texture0,
				const std::shared_ptr<texture_t<Dimension, T>>& texture1,
				const std::shared_ptr<texture_t<Dimension, real>>& alpha);

			void multiply(const T& value) override;

			auto copy_to() const->std::shared_ptr<texture_t<Dimension, T>> override;
			
			T sample(const surface_interaction& interaction) const override;

			T sample(const vector2& point) const override;
		private:
			std::shared_ptr<texture_t<Dimension, T>> mTexture0;
			std::shared_ptr<texture_t<Dimension, T>> mTexture1;

			std::shared_ptr<texture_t<Dimension, real>> mAlpha;
		};

		template <typename T>
		using mixture_texture2d = mixture_texture<2, T>;
	}
}

#include "detail/mixture_texture.hpp"