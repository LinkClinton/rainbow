#include "trowbridge_reitz_distribution.hpp"

#include "../scattering_function.hpp"

namespace rainbow::cpus::scatterings {

	vector2 trowbridge_reitz_sample(real cos_theta, real u1, real u2) {
		if (cos_theta > 0.9999) {
			const auto r = math::sqrt(u1 / (1 - u1));
			const auto phi = two_pi<real>() * u2;

			return vector2(r * cos(phi), r * sin(phi));
		}

		vector2 slope;

		const auto sin_theta = math::sqrt(math::max(static_cast<real>(0), 1 - cos_theta * cos_theta));
		const auto tan_theta = sin_theta / cos_theta;
		const auto a = 1 / tan_theta;
		const auto g1 = 2 / (1 + math::sqrt(1.f + 1.f / (a * a)));

		const auto A = 2 * u1 / g1 - 1;

		auto tmp = 1.f / (A * A - 1.f);
		if (tmp > 1e10) tmp = 1e10;

		const auto B = tan_theta;
		const auto D = math::sqrt(math::max(real(B * B * tmp * tmp - (A * A - B * B) * tmp), real(0)));
		const auto slope_x_1 = B * tmp - D;
		const auto slope_x_2 = B * tmp + D;

		slope.x = (A < 0 || slope_x_2 > 1.f / tan_theta) ? slope_x_1 : slope_x_2;

		real S;
		if (u2 > 0.5f) {
			S = 1.f;
			u2 = 2.f * (u2 - .5f);
		}
		else {
			S = -1.f;
			u2 = 2.f * (.5f - u2);
		}

		const auto z =
			(u2 * (u2 * (u2 * 0.27385f - 0.73369f) + 0.46341f)) /
			(u2 * (u2 * (u2 * 0.093073f + 0.309420f) - 1.000000f) + 0.597999f);

		slope.y = S * z * math::sqrt(1.f + slope.x * slope.x);

		return slope;
	}

	vector3 trowbridge_reitz_sample(const vector3& wi, real alpha_x, real alpha_y, real u1, real u2) {
		const auto wi_stretched = normalize(vector3(alpha_x * wi.x, alpha_y * wi.y, wi.z));

		auto slope = trowbridge_reitz_sample(cos_theta(wi_stretched), u1, u2);

		const auto tmp = cos_phi(wi_stretched) * slope.x - sin_phi(wi_stretched) * slope.y;
		slope.y = sin_phi(wi_stretched) * slope.x + cos_phi(wi_stretched) * slope.y;
		slope.x = tmp;

		slope.x = alpha_x * slope.x;
		slope.y = alpha_y * slope.y;

		return normalize(vector3(-slope.x, -slope.y, 1.));
	}

}

rainbow::cpus::scatterings::trowbridge_reitz_distribution::trowbridge_reitz_distribution(
	real alpha_x, real alpha_y, bool sample_visible_area) : microfacet_distribution(sample_visible_area),
	mAlphaX(alpha_x), mAlphaY(alpha_y)
{
}

rainbow::core::real rainbow::cpus::scatterings::trowbridge_reitz_distribution::distribution(const vector3& wh) const
{
	const auto tan_theta_2 = tan_theta_pow2(wh);
	
	if (std::isinf(tan_theta_2)) return 0;

	const auto cos_theta_4 = cos_theta_pow2(wh) * cos_theta_pow2(wh);
	const auto e = (
		cos_phi_pow2(wh) / (mAlphaX * mAlphaX) + 
		sin_phi_pow2(wh) / (mAlphaY * mAlphaY)) * tan_theta_2;
	
	return 1 / (pi<real>() * mAlphaX * mAlphaY * cos_theta_4 * (1 + e) * (1 + e));
}

rainbow::core::math::vector3 rainbow::cpus::scatterings::trowbridge_reitz_distribution::sample(const vector3& wo, const vector2& sample)
{
	if (mSampleVisibleArea) {
		const auto need_flip = wo.z < 0;
		const auto wh = trowbridge_reitz_sample(need_flip ? -wo : wo, mAlphaX, mAlphaY, sample.x, sample.y);

		if (need_flip) return -wh; else return wh;
	}
	
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

	cos_theta = 1 / math::sqrt(1 + tan_theta_2);

	const auto sin_theta = math::sqrt(max(static_cast<real>(0), 1 - cos_theta * cos_theta));
	const auto wh = spherical_direction(sin_theta, cos_theta, phi);

	return same_hemisphere(wo, wh) ? wh : -wh;
}

rainbow::core::real rainbow::cpus::scatterings::trowbridge_reitz_distribution::roughness_to_alpha(real roughness)
{
	roughness = max(roughness, static_cast<real>(1e-3));

	const auto x = log(roughness);

	return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x +
		0.000640711f * x * x * x * x;
}

rainbow::core::real rainbow::cpus::scatterings::trowbridge_reitz_distribution::lambda(const vector3& w) const
{
	const auto abs_tan_theta = math::abs(tan_theta(w));

	if (std::isinf(abs_tan_theta)) return 0;

	const auto alpha = math::sqrt(
		cos_phi_pow2(w) * mAlphaX * mAlphaX + sin_phi_pow2(w) * mAlphaY * mAlphaY);

	const auto alpha_tan_theta_2 = (alpha * abs_tan_theta) * (alpha * abs_tan_theta);

	return (-1 + math::sqrt(1.f + alpha_tan_theta_2)) / 2;
}
