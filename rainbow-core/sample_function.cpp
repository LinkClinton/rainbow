#include "sample_function.hpp"

rainbow::core::math::vector3 rainbow::core::uniform_sample_hemisphere(const vector2& sample)
{
	// when we know the z of vector, we can get the length of projected vector in x-y plane
	// it is sqrt(1 - z * z) in unit sphere
	// phi is the angle between projected vector in x-y plane and x-axis
	const auto z = sample.x;
	const auto r = sqrt(max(static_cast<real>(0), 1 - z * z));
	const auto phi = 2 * pi<real>() * sample.y;

	return vector3(r * cos(phi), r * sin(phi), z);
}

rainbow::core::math::vector3 rainbow::core::cosine_sample_hemisphere(const vector2& sample)
{
	// sample a point in a unit disk
	// the z of vector in unit sphere is 1 - x * x + y * y
	const auto disk = concentric_sample_disk(sample);
	const auto z = sqrt(max(static_cast<real>(0), 1 - dot(disk, disk)));

	return vector3(disk.x, disk.y, z);
}

rainbow::core::math::vector2 rainbow::core::concentric_sample_disk(const vector2& sample)
{
	// mapped sample from [0, 1] to [-1, 1]
	const auto sample_remapped = static_cast<real>(2) * sample - vector2(1);

	if (sample_remapped.x == 0 && sample_remapped.y == 0) return vector2(0);

	real theta;
	real r;

	if (abs(sample_remapped.x) > abs(sample_remapped.y)) {
		r = sample_remapped.x;
		theta = quarter_pi<real>() * (sample_remapped.y / sample_remapped.x);
	}else {
		r = sample_remapped.y;
		theta = half_pi<real>() - quarter_pi<real>() * (sample_remapped.x / sample_remapped.y);
	}

	return r * vector2(cos(theta), sin(theta));
}

rainbow::core::math::vector3 rainbow::core::uniform_sample_sphere(const vector2& sample)
{
	// remapped the sample.x in to [-1, 1] as the z of point
	const auto z = 1 - 2 * sample.x;

	// compute the length of radius projected into x-y plane
	const auto r = sqrt(max(static_cast<real>(0), 1 - z * z));
	const auto phi = 2 * two_pi<real>() * sample.y;
	return vector3(r * cos(phi), r * sin(phi), z);
}

rainbow::core::math::vector2 rainbow::core::uniform_sample_triangle(const vector2& sample)
{
	const auto u = sqrt(sample.x);

	return vector2(1 - u, sample.y * u);
}

rainbow::core::real rainbow::core::uniform_sample_hemisphere_pdf()
{
	return one_over_two_pi<real>();
}

rainbow::core::real rainbow::core::uniform_sample_cone_pdf(real cos_theta_max)
{
	return 1 / (two_pi<real>() * (1 - cos_theta_max));
}

rainbow::core::real rainbow::core::uniform_sample_sphere_pdf()
{
	return one_over_four_pi<real>();
}

rainbow::core::real rainbow::core::cosine_sample_hemisphere_pdf(real cos_theta)
{
	return cos_theta * one_over_pi<real>();
}

rainbow::core::real rainbow::core::sample_from_inv_cdf_table(const std::vector<real>& inv_cdf, real sample)
{
	assert(inv_cdf.size() >= 2);

	const auto size = inv_cdf.size() - 1;
	const auto which = clamp(
		static_cast<size_t>(sample * size), static_cast<size_t>(0), size - 1);

	const auto t = sample * size - which;

	return lerp(inv_cdf[which], inv_cdf[which + 1], t);
}

size_t rainbow::core::uniform_sample_one_from_range(size_t begin, size_t end, real sample)
{
	const auto size = end - begin;

	return clamp(static_cast<size_t>(sample * size), static_cast<size_t>(0), size - 1) + begin;
}

