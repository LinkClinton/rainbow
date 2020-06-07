#pragma once

#include "texture.hpp"

namespace rainbow::cpus::textures {

	template <size_t Dimension, typename T>
	class scale_texture_t final : public texture_t<Dimension, T> {
	public:
		explicit scale_texture_t(
			const std::shared_ptr<texture_t<Dimension, T>>& scale,
			const std::shared_ptr<texture_t<Dimension, T>>& base);

		void multiply(const T& value) override;

		auto copy_to() const->std::shared_ptr<texture_t<Dimension, T>> override;

		T sample(const surface_interaction& interaction) const override;

		T sample(const vector2& point) const override;
	private:
		std::shared_ptr<texture_t<Dimension, T>> mScale;
		std::shared_ptr<texture_t<Dimension, T>> mBase;
	};

	template <typename T>
	using scale_texture2d = scale_texture_t<2, T>;
	
}

#include "detail/scale_texture.hpp"