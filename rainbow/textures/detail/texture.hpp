#pragma once

#include "../texture.hpp"

namespace rainbow {

	namespace textures {

		template <size_t Dimension>
		texture_region_t<Dimension>::texture_region_t(const texture_size<Dimension>& size) :
			mSize(size)
		{
		}

		template <size_t Dimension>
		texture_size<Dimension> texture_region_t<Dimension>::size() const noexcept
		{
			return mSize;
		}

		template <>
		inline size_t texture_region_t<1>::index(const vector_t<1, size_t>& location) const
		{
			return location.x;
		}

		template <>
		inline size_t texture_region_t<2>::index(const vector_t<2, size_t>& location) const
		{
			return location.y * mSize.x + location.x;
		}

		template <>
		inline size_t texture_region_t<1>::length() const noexcept
		{
			return mSize.x;
		}

		template <>
		inline size_t texture_region_t<2>::length() const noexcept
		{
			return mSize.x * mSize.y;
		}

		template <size_t Dimension, typename T>
		texture_t<Dimension, T>::texture_t(const vector_t<Dimension, size_t>& size) :
			texture_region_t<Dimension>(size), texture_sample_t<T>()
		{
		}
	}
}
