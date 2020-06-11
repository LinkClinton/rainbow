#pragma once

#include "../image_texture.hpp"

using namespace rainbow::core;

namespace rainbow::cpus::textures {
	
	template <typename T>
	T lerp(const T& s, const T& t, real x)
	{
		return s * (1 - x) + t * x;
	}

	template <typename T>
	image_texture_t<2, T>::image_texture_t(const vector_t<2, size_t>& size, const std::vector<T>& values) :
		texture_t<2, T>(size), mValues(values)
	{
		assert(mValues.size() >= texture_region_t<2>::length());
	}
	
	template <typename T>
	image_texture_t<2, T>::image_texture_t(const vector_t<2, size_t>& size) :
		texture_t<2, T>(size), mValues(texture_region_t<2>::length())
	{
	}
	
	template <typename T>
	void image_texture_t<2, T>::multiply(const T& value)
	{
		for (size_t index = 0; index < mValues.size(); index++)
			mValues[index] *= value;
	}

	template <typename T>
	auto image_texture_t<2, T>::copy_to() const -> std::shared_ptr<texture_t<2, T>>
	{
		return std::make_shared<image_texture_t<2, T>>(this->mSize, this->mValues);
	}

	template <typename T>
	T image_texture_t<2, T>::sample(const surface_interaction& interaction) const
	{
		return sample(interaction.uv);
	}

	template <typename T>
	T image_texture_t<2, T>::sample(const vector_t<2, real>& point) const
	{
		const auto uv = vector2(
			mod(point.x, static_cast<real>(1)) * this->mSize.x - 0.5,
			mod(point.y, static_cast<real>(1)) * this->mSize.y - 0.5);

		const auto min_range = vector2i(0);
		const auto max_range = vector2i(this->mSize.x - 1, this->mSize.y - 1);

		// find the four point near the uv point
		const auto uv00 = clamp(vector2i(floor(uv.x), floor(uv.y)), min_range, max_range);

		const auto uv01 = clamp(vector2i(uv00.x + 0, uv00.y + 1), min_range, max_range);
		const auto uv10 = clamp(vector2i(uv00.x + 1, uv00.y + 0), min_range, max_range);
		const auto uv11 = clamp(vector2i(uv00.x + 1, uv00.y + 1), min_range, max_range);

		// find the delta value of uv
		const auto du = clamp(uv.x - uv00.x, static_cast<real>(0), static_cast<real>(1));
		const auto dv = clamp(uv.y - uv00.y, static_cast<real>(0), static_cast<real>(1));

		const auto v0 = textures::lerp(
			mValues[texture_region_t<2>::index(uv00)],
			mValues[texture_region_t<2>::index(uv10)], du);

		const auto v1 = textures::lerp(
			mValues[texture_region_t<2>::index(uv01)],
			mValues[texture_region_t<2>::index(uv11)], du);

		return textures::lerp(v0, v1, dv);
	}

	template <typename T>
	T image_texture_t<2, T>::value(size_t index) const
	{
		return mValues[index];
	}

	template <typename T>
	image_texture_t<3, T>::image_texture_t(const vector_t<3, size_t>& size, const std::vector<T>& values) :
		texture_t<3, T>(size), mValues(values)
	{
		assert(mValues.size() >= texture_region_t<3>::length());
	}

	template <typename T>
	image_texture_t<3, T>::image_texture_t(const vector_t<3, size_t>& size) :
		texture_t<3, T>(size), mValues(texture_region_t<3>::length())
	{
	}

	template <typename T>
	void image_texture_t<3, T>::multiply(const T& value)
	{
		for (size_t index = 0; index < mValues.size(); index++)
			mValues[index] *= value;
	}

	template <typename T>
	auto image_texture_t<3, T>::copy_to() const -> std::shared_ptr<texture_t<3, T>>
	{
		return std::make_shared<image_texture_t<3, T>>(this->mSize, this->mValues);
	}

	template <typename T>
	T image_texture_t<3, T>::sample(const surface_interaction& interaction) const
	{
		// for interaction, we will sample the point of interaction
		return sample(interaction.point);
	}

	template <typename T>
	T image_texture_t<3, T>::sample(const vector_t<3, real>& point) const
	{
		const auto uv = vector3(
			mod(point.x, static_cast<real>(1)) * this->mSize.x - 0.5,
			mod(point.y, static_cast<real>(1)) * this->mSize.y - 0.5,
			mod(point.z, static_cast<real>(1)) * this->mSize.z - 0.5);

		const auto min_range = vector3i(0);
		const auto max_range = vector3i(this->mSize.x - 1, this->mSize.y - 1, this->mSize.z - 1);

		const auto uv000 = clamp(vector3i(floor(uv.x), floor(uv.y), floor(uv.z)), min_range, max_range);
		const auto uv001 = clamp(vector3i(uv000.x + 0, uv000.y + 0, uv000.z + 1), min_range, max_range);
		const auto uv010 = clamp(vector3i(uv000.x + 0, uv000.y + 1, uv000.z + 0), min_range, max_range);
		const auto uv011 = clamp(vector3i(uv000.x + 0, uv000.y + 1, uv000.z + 1), min_range, max_range);
		const auto uv100 = clamp(vector3i(uv000.x + 1, uv000.y + 0, uv000.z + 0), min_range, max_range);
		const auto uv101 = clamp(vector3i(uv000.x + 1, uv000.y + 0, uv000.z + 1), min_range, max_range);
		const auto uv110 = clamp(vector3i(uv000.x + 1, uv000.y + 1, uv000.z + 0), min_range, max_range);
		const auto uv111 = clamp(vector3i(uv000.x + 1, uv000.y + 1, uv000.z + 1), min_range, max_range);

		const auto dx = math::clamp(uv.x - uv000.x, static_cast<real>(0), static_cast<real>(1));
		const auto dy = math::clamp(uv.y - uv000.y, static_cast<real>(0), static_cast<real>(1));
		const auto dz = math::clamp(uv.z - uv000.z, static_cast<real>(0), static_cast<real>(1));

		const auto uv00 = textures::lerp(
			mValues[texture_region_t<3>::index(uv000)],
			mValues[texture_region_t<3>::index(uv100)], dx);

		const auto uv01 = textures::lerp(
			mValues[texture_region_t<3>::index(uv001)],
			mValues[texture_region_t<3>::index(uv101)], dx);

		const auto uv10 = textures::lerp(
			mValues[texture_region_t<3>::index(uv010)],
			mValues[texture_region_t<3>::index(uv110)], dx);

		const auto uv11 = textures::lerp(
			mValues[texture_region_t<3>::index(uv011)],
			mValues[texture_region_t<3>::index(uv111)], dx);

		const auto v0 = textures::lerp(uv00, uv10, dy);
		const auto v1 = textures::lerp(uv01, uv11, dy);

		return textures::lerp(v0, v1, dz);
	}

	template <typename T>
	T image_texture_t<3, T>::value(size_t index) const
	{
		return mValues[index];
	}

}
