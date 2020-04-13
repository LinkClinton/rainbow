#pragma once

#include "../constant_texture.hpp"

namespace rainbow {

	namespace textures {

		template <size_t Dimension, typename T>
		constant_texture_t<Dimension, T>::constant_texture_t(const T& value, const vector_t<Dimension, size_t>& size) :
			texture_t<Dimension, T>(size), mValue(value)
		{
		}

		template <size_t Dimension, typename T>
		T constant_texture_t<Dimension, T>::sample(const surface_interaction& interaction)
		{
			return mValue;
		}

	}
}
