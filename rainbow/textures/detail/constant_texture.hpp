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
