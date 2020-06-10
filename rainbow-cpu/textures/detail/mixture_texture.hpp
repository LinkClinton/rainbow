#pragma once

#include "../mixture_texture.hpp"

namespace rainbow::cpus::textures {

	template <size_t Dimension, typename T>
	mixture_texture_t<Dimension, T>::mixture_texture_t(
		const std::shared_ptr<texture_t<Dimension, T>>& texture0,
		const std::shared_ptr<texture_t<Dimension, T>>& texture1,
		const std::shared_ptr<texture_t<Dimension, real>>& alpha) :
		texture_t<Dimension, T>(vector_t<Dimension, size_t>(0)),
		mTexture0(texture0), mTexture1(texture1), mAlpha(alpha)
	{
	}

	template <size_t Dimension, typename T>
	void mixture_texture_t<Dimension, T>::multiply(const T& value)
	{
		mTexture0->multiply(value);
		mTexture1->multiply(value);
	}

	template <size_t Dimension, typename T>
	auto mixture_texture_t<Dimension, T>::copy_to() const -> std::shared_ptr<texture_t<Dimension, T>>
	{
		return std::make_shared<mixture_texture_t>(mTexture0->copy_to(), mTexture1->copy_to(), mAlpha->copy_to());
	}

	template <size_t Dimension, typename T>
	T mixture_texture_t<Dimension, T>::sample(const surface_interaction& interaction) const
	{
		return sample(interaction.point);
	}

	template <size_t Dimension, typename T>
	T mixture_texture_t<Dimension, T>::sample(const vector_t<Dimension, real>& point) const
	{
		const auto alpha0 = clamp(mAlpha->sample(point), static_cast<real>(0), static_cast<real>(1));
		const auto alpha1 = clamp(1 - alpha0, static_cast<real>(0), static_cast<real>(1));

		return mTexture0->sample(point) * alpha0 + mTexture1->sample(point) * alpha1;
	}

}
