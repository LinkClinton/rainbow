#include "microfacet_reflection.hpp"

rainbow::scatterings::microfacet_reflection::microfacet_reflection(
	const std::shared_ptr<microfacet_distribution>& distribution, 
	const std::shared_ptr<fresnel_effect>& fresnel,
	const spectrum& reflectance) : reflection_function(scattering_type::glossy, reflectance),
	mDistribution(distribution), mFresnel(fresnel)
{
}

rainbow::spectrum rainbow::scatterings::microfacet_reflection::evaluate(const vector3& wo, const vector3& wi) const
{
	const auto cos_theta_o = abs(cos_theta(wo));
	const auto cos_theta_i = abs(cos_theta(wi));

	auto wh = wi + wo;

	// degenerate cases for microfacet reflection
	if (cos_theta_o == 0 || cos_theta_i == 0) return 0;
	
	if (wh.x == 0 && wh.y == 0 && wh.z == 0) return 0;

	wh = normalize(wh);

	const auto fresnel = mFresnel->evaluate(dot(wi, face_forward(wh, vector3(0, 0, 1))));
	const auto masking_shadowing = mDistribution->masking_shadowing(wo, wi);
	const auto distribution = mDistribution->distribution(wh);
	
	return mReflectance * (masking_shadowing * distribution) * fresnel /
		(4 * cos_theta_i * cos_theta_o);
}

rainbow::scatterings::scattering_sample rainbow::scatterings::microfacet_reflection::sample(
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

rainbow::real rainbow::scatterings::microfacet_reflection::pdf(const vector3& wo, const vector3& wi) const
{
	if (!same_hemisphere(wo, wi)) return 0;

	const auto wh = normalize(wo + wi);

	return mDistribution->pdf(wo, wh) / (4 * dot(wo, wh));
}
