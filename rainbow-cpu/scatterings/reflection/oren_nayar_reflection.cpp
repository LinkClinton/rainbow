#include "oren_nayar_reflection.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::scatterings::oren_nayar_reflection::oren_nayar_reflection(const spectrum& reflectance, real sigma) :
	reflection_function(scattering_type::diffuse, reflectance)
{
	const auto sigma_radians = radians(sigma);
	const auto sigma_2 = sigma_radians * sigma_radians;

	// f(wo, wi) = R \ pi * (A + B * max(0, cos(phi_i - phi_o)) * sin(alpha) * tan(beta)
	// A = 1 - sigma * sigma / 2 * (sigma * sigma + 0.33)
	// B = 0.45 * sigma * sigma / (sigma * sigma + 0.09)
	mA = static_cast<real>(1. - (sigma_2 / (2 * (sigma_2 + 0.33))));
	mB = static_cast<real>(0.45 * sigma_2 / (sigma_2 + 0.09));
}

spectrum rainbow::cpus::scatterings::oren_nayar_reflection::evaluate(const vector3& wo, const vector3& wi) const
{
	// f(wo, wi) = R \ pi * (A + B * max(0, cos(phi_i - phi_o)) * sin(alpha) * tan(beta)
	// alpha = max(theta_i, theta_o)
	// beta = min(theta_i, theta_o)

	const auto sin_theta_i = sin_theta(wi);
	const auto sin_theta_o = sin_theta(wo);

	real max_cos = 0;
	if (sin_theta_i > 1e-4 && sin_theta_o > 1e-4) {
		const auto sin_phi_i = sin_phi(wi); const auto sin_phi_o = sin_phi(wo);
		const auto cos_phi_i = cos_phi(wi); const auto cos_phi_o = cos_phi(wo);

		max_cos = max(static_cast<real>(0), cos_phi_i * cos_phi_o + sin_phi_i * sin_phi_o);
	}

	real sin_alpha = 0;
	real tan_beta = 0;

	if (math::abs(cos_theta(wi)) > math::abs(cos_theta(wo))) {
		sin_alpha = sin_theta_o;
		tan_beta = sin_theta_i / math::abs(cos_theta(wi));
	}else {
		sin_alpha = sin_theta_i;
		tan_beta = sin_theta_o / math::abs(cos_theta(wo));
	}

	return mReflectance * one_over_pi<real>() * (mA + mB * max_cos * sin_alpha * tan_beta);
}
