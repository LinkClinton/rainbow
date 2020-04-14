#include "trowbridge_reitz_distribution.hpp"

#include "../scattering_function.hpp"

rainbow::scatterings::trowbridge_reitz_distribution::trowbridge_reitz_distribution(
	real alpha_x, real alpha_y, bool sample_visible_area) : microfacet_distribution(sample_visible_area),
	mAlphaX(alpha_x), mAlphaY(alpha_y)
{
}

rainbow::real rainbow::scatterings::trowbridge_reitz_distribution::distribution(const vector3& wh) const
{
	const auto tan_theta_2 = tan_theta_pow2(wh);
	
	if (std::isinf(tan_theta_2)) return 0;

	const auto cos_theta_4 = cos_theta_pow2(wh) * cos_theta_pow2(wh);
	const auto e = (
		cos_phi_pow2(wh) / (mAlphaX * mAlphaX) + 
		sin_phi_pow2(wh) / (mAlphaY * mAlphaY)) * tan_theta_2;
	
	return 1 / (pi<real>() * mAlphaX * mAlphaY * cos_theta_4 * (1 + e) * (1 + e));
}

rainbow::vector3 rainbow::scatterings::trowbridge_reitz_distribution::sample(const vector3& wo, const vector2& sample)
{
	if (mSampleVisibleArea) throw std::exception("not implementation.");

	real tan_theta_2 = 0;
	real cos_theta = 0;
	auto phi = two_pi<real>() * sample.y;

	if (mAlphaX == mAlphaY)
		tan_theta_2 = mAlphaX * mAlphaX * sample.x / (1 - sample.x);
	else {
		phi = atan(mAlphaY / mAlphaX * tan(phi + half_pi<real>()));

		if (sample.y > 0.5) phi += pi<real>();

		const auto sin_phi = sin(phi);
		const auto cos_phi = cos(phi);

		const auto alpha_x_2 = mAlphaX * mAlphaX;
		const auto alpha_y_2 = mAlphaY * mAlphaY;
		const auto alpha_2 = 1 / (cos_phi * cos_phi / alpha_x_2 + sin_phi * sin_phi / alpha_y_2);
		
		tan_theta_2 = alpha_2 * sample.x / (1 - sample.x);
	}

	cos_theta = 1 / sqrt(1 + tan_theta_2);

	const auto sin_theta = sqrt(max(static_cast<real>(0), 1 - cos_theta * cos_theta));
	const auto wh = spherical_direction(sin_theta, cos_theta, phi);

	return same_hemisphere(wo, wh) ? wh : -wh;
}

rainbow::real rainbow::scatterings::trowbridge_reitz_distribution::roughness_to_alpha(real roughness)
{
	roughness = max(roughness, static_cast<real>(1e-3));

	const auto x = log(roughness);

	return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x +
		0.000640711f * x * x * x * x;
}

rainbow::real rainbow::scatterings::trowbridge_reitz_distribution::lambda(const vector3& w) const
{
	const auto abs_tan_theta = abs(tan_theta(w));

	if (std::isinf(abs_tan_theta)) return 0;

	const auto alpha = sqrt(
		cos_phi_pow2(w) * mAlphaX * mAlphaX + sin_phi_pow2(w) * mAlphaY * mAlphaY);

	const auto alpha_tan_theta_2 = (alpha * abs_tan_theta) * (alpha * abs_tan_theta);

	return (-1 + sqrt(1.f + alpha_tan_theta_2)) / 2;
}
