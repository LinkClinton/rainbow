#pragma once

#include "../texture.hpp"

namespace rainbow {

	namespace textures {

		template <size_t Dimension, typename T>
		texture_t<Dimension, T>::texture_t(const vector_t<Dimension, size_t>& size) : mSize(size)
		{
		}

		template <size_t Dimension, typename T>
		vector_t<Dimension, size_t> texture_t<Dimension, T>::size() const noexcept
		{
			return mSize;
		}

	}
}
