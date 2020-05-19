#pragma once

#include "texture.hpp"

namespace rainbow {

	namespace textures {

		template <size_t Dimension, typename T>
		class image_texture_t;
		
		template <typename T>
		class image_texture_t<2, T> final : public texture_t<2, T> {
		public:
			explicit image_texture_t(const vector_t<2, size_t>& size);

			explicit image_texture_t(
				const vector_t<2, size_t>& size, const std::vector<T>& values);
			
			~image_texture_t() = default;

			void multiply(const T& value) override;
			
			T sample(const surface_interaction& interaction) const override;

			T sample(const vector2& point) const override;
		private:
			std::vector<T> mValues;
		};

		template <typename T>
		using image_texture2d = image_texture_t<2, T>;
	}
}

#include "detail/image_texture.hpp"