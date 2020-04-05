#pragma once

#include "../texture.hpp"

namespace rainbow {

	namespace textures {

		template <size_t Dimension, typename T>
		texture_t<2, T>::texture_t(size_t width, size_t height) : mWidth(width), mHeight(height)
		{
			
		}

		template <size_t Dimension, typename T>
		size_t texture_t<2, T>::width() const noexcept
		{
			return mWidth;
		}

		template <size_t Dimension, typename T>
		size_t texture_t<2, T>::height() const noexcept
		{
			return mHeight;
		}

	}
}
