#pragma once

#include "../constant_texture.hpp"

namespace rainbow {

	namespace textures {

		template <size_t Dimension, typename T>
		constant_texture_t<Dimension, T>::constant_texture_t(const T& value, const texture_size<Dimension>& size) :
			texture_t<Dimension, T>(size), mValue(value)
		{
		}

		template <size_t Dimension, typename T>
		void constant_texture_t<Dimension, T>::multiply(const T& value)
		{
			mValue *= value;
		}

		template <size_t Dimension, typename T>
		auto constant_texture_t<Dimension, T>::copy_to() const -> std::shared_ptr<texture_t<Dimension, T>>
		{
			return std::make_shared<constant_texture_t<Dimension, T>>(this->mValue, this->mSize);
		}

		template <size_t Dimension, typename T>
		T constant_texture_t<Dimension, T>::sample(const surface_interaction& interaction) const
		{
			return mValue;
		}

		template <size_t Dimension, typename T>
		T constant_texture_t<Dimension, T>::sample(const vector2& point) const
		{
			static_assert(Dimension == 2, "The Dimension should be 2!");
			
			return mValue;
		}

	}
}
