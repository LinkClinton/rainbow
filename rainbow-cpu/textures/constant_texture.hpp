#pragma once

#include "texture.hpp"

namespace rainbow::cpus::textures {

	template <size_t Dimension, typename T>
	class constant_texture_t final : public texture_t<Dimension, T> {
	public:
		explicit constant_texture_t(const T& value, const texture_size<Dimension>& size = texture_size<Dimension>(1));

		void multiply(const T& value) override;

		auto copy_to() const->std::shared_ptr<texture_t<Dimension, T>> override;

		T sample(const surface_interaction& interaction) const override;

		T sample(const vector_t<Dimension, real>& point) const override;
	private:
		T mValue;
	};

	template <typename T>
	using constant_texture2d = constant_texture_t<2, T>;

	template <typename T>
	using constant_texture3d = constant_texture_t<3, T>;
}

#include "detail/constant_texture.hpp"