#include "microfacet_transmission.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::scatterings::microfacet_transmission::microfacet_transmission(
	const std::shared_ptr<microfacet_distribution>& distribution, const transport_mode& mode, const spectrum& transmission,
	real eta_i, real eta_o) : transmission_function(scattering_type::glossy, mode, transmission),
	mDistribution(distribution),
	mFresnel(std::make_shared<fresnel_effect_dielectric>(eta_i, eta_o)),
	mEtaI(eta_i), mEtaO(eta_o)
{
}

spectrum rainbow::cpus::scatterings::microfacet_transmission::evaluate(const vector3& wo, const vector3& wi) const
{
	if (same_hemisphere(wo, wi)) return {};

	const auto cos_theta_o = cos_theta(wo);
	const auto cos_theta_i = cos_theta(wi);

	if (cos_theta_o == 0 || cos_theta_i == 0) return {};

	const auto entering = cos_theta_o > 0;
	const auto eta = entering ? (mEtaO / mEtaI) : (mEtaI / mEtaO);

	const auto wh = face_forward(normalize(wo + wi * eta), vector3(0, 0, 1));

	const auto fresnel = mFresnel->evaluate(dot(wo, wh));
	const auto denominator = dot(wo, wh) + eta * dot(wi, wh);

	const auto factor = mMode == transport_mode::radiance ? (1 / eta) : 1;
	const auto distribution = mDistribution->distribution(wh);
	const auto masking_shadowing = mDistribution->masking_shadowing(wo, wi);
	
	return (spectrum(1) - fresnel) * mTransmission * math::abs(
		distribution * masking_shadowing * eta * eta * math::abs(dot(wi, wh)) * math::abs(dot(wo, wh) * factor * factor) /
		(cos_theta_i * cos_theta_o * denominator * denominator));
}

rainbow::cpus::scatterings::scattering_sample rainbow::cpus::scatterings::microfacet_transmission::sample(
	const vector3& wo, const vector2& sample) const
{
	if (wo.z == 0) return {};

	const auto wh = mDistribution->sample(wo, sample);

	if (dot(wo, wh) < 0) return {};

	const auto entering = cos_theta(wo) > 0;
	const auto eta = entering ? (mEtaI / mEtaO) : (mEtaO / mEtaI);

	const auto wi = refract(wo, wh, eta);

	if (wi == vector3(0)) return {};

	return scattering_sample(
		mType,
		evaluate(wo, wi),
		wi,
		pdf(wo, wi)
	);
}

rainbow::core::real rainbow::cpus::scatterings::microfacet_transmission::pdf(const vector3& wo, const vector3& wi) const
{
	if (same_hemisphere(wo, wi)) return 0;

	const auto entering = cos_theta(wo) > 0;
	const auto eta = entering ? (mEtaO / mEtaI) : (mEtaI / mEtaO);

	const auto wh = normalize(wo + wi * eta);
	const auto denominator = dot(wo, wh) + eta * dot(wi, wh);

	return mDistribution->pdf(wo, wh) * math::abs(
		(eta * eta * dot(wi, wh)) / (denominator * denominator));
}
