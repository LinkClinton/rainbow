#pragma once

#include "../image_texture.hpp"

namespace rainbow {

	namespace textures {

		template <typename T>
		T lerp(const T& s, const T& t, real x)
		{
			return s * (1 - x) + t * s;
		}

		template <typename T>
		image_texture_t<2, T>::image_texture_t(const vector_t<2, size_t>& size) :
			texture_t<2, T>(size), mValues(texture_region_t<2>::length())
		{
		}

		template <typename T>
		image_texture_t<2, T>::image_texture_t(const vector_t<2, size_t>& size, const std::vector<T>& values) :
			texture_t<2, T>(size), mValues(values)
		{
			assert(mValues.size() >= texture_region_t<2>::length());
		}

		template <typename T>
		T image_texture_t<2, T>::sample(const surface_interaction& interaction) const
		{
			const auto uv = vector2(
				interaction.uv.x * this->mSize.x,
				interaction.uv.y * this->mSize.y);

			// find the four point near the uv point
			const auto uv00 = vector2i(floor(uv.x), floor(uv.y));
			const auto uv01 = vector2i(floor(uv.x), ceil(uv.y));
			const auto uv10 = vector2i(ceil(uv.x), floor(uv.y));
			const auto uv11 = vector2i(ceil(uv.x), ceil(uv.y));

			// find the delta value of uv
			const auto du = uv.x - uv00.x;
			const auto dv = uv.y - uv00.y;

			const auto v0 = lerp(
				mValues[texture_region_t<2>::index(uv00)],
				mValues[texture_region_t<2>::index(uv01)], du);

			const auto v1 = lerp(
				mValues[texture_region_t<2>::index(uv10)],
				mValues[texture_region_t<2>::index(uv11)], du);

			return lerp(v0, v1, dv);
		}

	}
}
