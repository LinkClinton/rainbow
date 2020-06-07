#pragma once

#include "../scale_texture.hpp"

namespace rainbow::cpus::textures {

	template <size_t Dimension, typename T>
	scale_texture_t<Dimension, T>::scale_texture_t(
		const std::shared_ptr<texture_t<Dimension, T>>& scale,
		const std::shared_ptr<texture_t<Dimension, T>>& base) :
		texture_t<Dimension, T>(vector_t<Dimension, size_t>(0)),
		mScale(scale), mBase(base)
	{
	}

	template <size_t Dimension, typename T>
	void scale_texture_t<Dimension, T>::multiply(const T& value)
	{
		mScale->multiply(value);
		mBase->multiply(value);
	}

	template <size_t Dimension, typename T>
	auto scale_texture_t<Dimension, T>::copy_to() const -> std::shared_ptr<texture_t<Dimension, T>>
	{
		return std::make_shared<scale_texture_t<Dimension, T>>(mScale->copy_to(), mBase->copy_to());
	}

	template <size_t Dimension, typename T>
	T scale_texture_t<Dimension, T>::sample(const surface_interaction& interaction) const
	{
		return sample(interaction.uv);
	}

	template <size_t Dimension, typename T>
	T scale_texture_t<Dimension, T>::sample(const vector2& point) const
	{
		return mBase->sample(point) * mScale->sample(point);
	}
}
