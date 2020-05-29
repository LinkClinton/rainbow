#include "fresnel_blend_reflection.hpp"

#include "../../shared/sample_function.hpp"

rainbow::scatterings::fresnel_blend_reflection::fresnel_blend_reflection(
	const std::shared_ptr<microfacet_distribution>& distribution, 
	const spectrum& reflectance_specular,
	const spectrum& reflectance_diffuse,
	const spectrum& scale) : reflection_function(scattering_type::glossy, spectrum(1)),
	mDistribution(distribution), mReflectanceSpecular(reflectance_specular), mReflectanceDiffuse(reflectance_diffuse), mScale(scale)
{
	// this function model two layers
	// the top layer is glossy layer, the bottom layer is diffuse layer
	// when we see the layer with small theta(the angle between normal and wo)
	// most light is transmitted to the diffuse layer (due to fresnel effect)
	// when we see the layer with big theta
	// most light is reflected with glossy layer
	//
	// schlick fresnel = F + (1 - F) * pow_5(1 - cos_theta)
	// F is the reflectance when we it with theta = 0
	// cos_theta is the angle between with wo and normal
}

rainbow::spectrum rainbow::scatterings::fresnel_blend_reflection::evaluate(const vector3& wo, const vector3& wi) const
{
	auto wh = wo + wi;

	if (wh == vector3(0)) return spectrum(0);

	wh = normalize(wh);

	// fr(wo, wi) = rd * (28 / (23 * pi)) * (1 - rs) * (1 - pow_5(1 - dot(wi, n) / 2)) * (1 - pow_5(1 - dot(wo, n) / 2))
	const auto diffuse = mReflectanceDiffuse * (28 / (23 * pi<real>())) *
		(spectrum(1) - mReflectanceSpecular) *
		(1 - pow_5(1 - static_cast<real>(0.5) * abs(cos_theta(wi)))) *
		(1 - pow_5(1 - static_cast<real>(0.5) * abs(cos_theta(wo))));

	// fr(wo, wi) = distribution * fresnel / (4 * dot(wi, wh) * max(dot(wi, n), dot(wo, n)))
	const auto distribution = mDistribution->distribution(wh);
	const auto fresnel = schlick_fresnel(dot(wo, wh));
	const auto specular = fresnel * distribution /
		(4 * abs(dot(wi, wh)) * max(abs(cos_theta(wi)), abs(cos_theta(wo))));

	return mScale * (specular + diffuse);
}

rainbow::scatterings::scattering_sample rainbow::scatterings::fresnel_blend_reflection::sample(
	const vector3& wo, const vector2& sample) const
{
	if (sample.x < 0.5) {
		
		const auto sample_remapped = vector2(min(2 * sample.x, one_minus_epsilon<real>()), sample.y);
		auto wi = cosine_sample_hemisphere(sample_remapped);

		if (wo.z < 0) wi.z *= -1;

		return scattering_sample(mType, evaluate(wo, wi), wi, pdf(wo, wi));
	}

	const auto sample_remapped = vector2(min(2 * (sample.x - static_cast<real>(0.5)), one_minus_epsilon<real>()), sample.y);
	const auto wh = mDistribution->sample(wo, sample_remapped);
	const auto wi = reflect(wo, wh);

	if (!same_hemisphere(wo, wi)) return {};

	return scattering_sample(mType, evaluate(wo, wi), wi, pdf(wo, wi));
}

rainbow::real rainbow::scatterings::fresnel_blend_reflection::pdf(const vector3& wo, const vector3& wi) const
{
	if (!same_hemisphere(wo, wi)) return 0;

	const auto wh = normalize(wo + wi);

	const auto sample_distribution_pdf = mDistribution->pdf(wo, wh) / (4 * dot(wo, wh));
	const auto sample_hemisphere_pdf = cosine_sample_hemisphere_pdf(abs(cos_theta(wi)));

	return static_cast<real>(0.5) * (sample_distribution_pdf + sample_hemisphere_pdf);
}

rainbow::spectrum rainbow::scatterings::fresnel_blend_reflection::schlick_fresnel(real cos_theta) const noexcept
{
	return mReflectanceSpecular + (spectrum(1) - mReflectanceSpecular) * pow_5(1 - cos_theta);
}

rainbow::real rainbow::scatterings::fresnel_blend_reflection::pow_5(real value) const noexcept
{
	return (value * value) * (value * value) * value;
}
