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
				fmod(interaction.uv.x, 1) * (this->mSize.x - 1),
				fmod(interaction.uv.y, 1) * (this->mSize.y - 1));

			const auto min_range = vector2i(0);
			const auto max_range = vector2i(this->mSize.x - 1, this->mSize.y - 1);
			
			// find the four point near the uv point
			const auto uv00 = clamp(vector2i(floor(uv.x), floor(uv.y)), min_range, max_range);
			const auto uv01 = clamp(vector2i(floor(uv.x), ceil(uv.y)), min_range, max_range);
			const auto uv10 = clamp(vector2i(ceil(uv.x), floor(uv.y)), min_range, max_range);
			const auto uv11 = clamp(vector2i(ceil(uv.x), ceil(uv.y)), min_range, max_range);

			// find the delta value of uv
			const auto du = clamp(uv.x - uv00.x, 0.f, 1.f);
			const auto dv = clamp(uv.y - uv00.y, 0.f, 1.f);

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
