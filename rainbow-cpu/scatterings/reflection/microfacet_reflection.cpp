#include "microfacet_reflection.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::scatterings::microfacet_reflection::microfacet_reflection(
	const std::shared_ptr<microfacet_distribution>& distribution, 
	const std::shared_ptr<fresnel_effect>& fresnel,
	const spectrum& reflectance) : reflection_function(scattering_type::glossy, reflectance),
	mDistribution(distribution), mFresnel(fresnel)
{
}

spectrum rainbow::cpus::scatterings::microfacet_reflection::evaluate(const vector3& wo, const vector3& wi) const
{
	const auto cos_theta_o = math::abs(cos_theta(wo));
	const auto cos_theta_i = math::abs(cos_theta(wi));

	auto wh = wi + wo;

	// degenerate cases for microfacet reflection
	if (cos_theta_o == 0 || cos_theta_i == 0) return 0;
	
	if (wh.x == 0 && wh.y == 0 && wh.z == 0) return 0;

	// the space where the surface normal(0, 0, 1) in is the eta_i space
	// when the wo and wi are in the eta_o space, we need swap the eta_i and eta_o of fresnel
	// because the eta_o is the real eta_i for computing the function
	// in fresnel effect, if the cos_theta_i is less than 0 we will swap the eta_i and eta_o
	// so the wh should be the same hemisphere as surface normal
	// the dot(wh, wi) > 0 means the wi is in the eta_i space, so we do not need swap them.
	// the dot(wh, wi) < 0 means the wi is not in the eta_i space, so we need swap them.
	wh = face_forward(normalize(wh), vector3(0, 0, 1));

	const auto fresnel = mFresnel->evaluate(dot(wi, wh));
	const auto masking_shadowing = mDistribution->masking_shadowing(wo, wi);
	const auto distribution = mDistribution->distribution(wh);
	
	return mReflectance * (masking_shadowing * distribution) * fresnel /
		(4 * cos_theta_i * cos_theta_o);
}

rainbow::cpus::scatterings::scattering_sample rainbow::cpus::scatterings::microfacet_reflection::sample(
	const vector3& wo, const vector2& sample) const
{
	if (wo.z == 0) return {};

	const auto wh = mDistribution->sample(wo, sample);

	// if the wo and wh are not in the same hemisphere, we will return 0
	// because the normal is not the surface normal, we will use dot instead of same_hemisphere
	if (dot(wo, wh) < 0) return {};

	const auto wi = reflect(wo, wh);

	// if the wo and wi are not in the same hemisphere, we will return 0
	if (!same_hemisphere(wo, wi)) return {};

	return scattering_sample(
		mType,
		evaluate(wo, wi),
		wi,
		mDistribution->pdf(wo, wh) / (4 * dot(wo, wh))
	);
}

rainbow::core::real rainbow::cpus::scatterings::microfacet_reflection::pdf(const vector3& wo, const vector3& wi) const
{
	if (!same_hemisphere(wo, wi)) return 0;

	const auto wh = normalize(wo + wi);

	return mDistribution->pdf(wo, wh) / (4 * dot(wo, wh));
}
