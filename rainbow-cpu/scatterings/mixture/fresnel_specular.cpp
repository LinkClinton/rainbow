#include "fresnel_specular.hpp"

#include "../fresnel_effect.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::scatterings::fresnel_specular::fresnel_specular(
	const transport_mode& mode, const spectrum& transmission, const spectrum& reflectance,
	real eta_i, real eta_o) : bidirectional_scattering_distribution_function(
		scattering_type::reflection | scattering_type::transmission | scattering_type::specular),
	mTransmission(transmission), mReflectance(reflectance), mMode(mode), mEtaI(eta_i), mEtaO(eta_o)
{
	/*
	 * fresnel specular is a bsdf that model specular reflectance and specular transmission with special weight
	 * the special weight of reflectance and transmission is the value of Fresnel Equation
	 * (it indicate the amount of light was reflected)
	 */
}

spectrum rainbow::cpus::scatterings::fresnel_specular::evaluate(const vector3& wo, const vector3& wi) const
{
	return 0;
}

rainbow::cpus::scatterings::scattering_sample rainbow::cpus::scatterings::fresnel_specular::sample(const vector3& wo,
	const vector2& sample) const
{
	const auto fresnel = fresnel_reflect_dielectric(cos_theta(wo), mEtaI, mEtaO);

	// specular reflection
	if (sample.x < fresnel) {
		const auto wi = vector3(-wo.x, -wo.y, wo.z);

		return scattering_sample(
			scattering_type::reflection | scattering_type::specular,
			mReflectance * fresnel / math::abs(cos_theta(wi)), wi, fresnel);
	}

	// specular transmission
	const auto entering = cos_theta(wo) > 0;
	const auto eta_i = entering ? mEtaI : mEtaO;
	const auto eta_o = entering ? mEtaO : mEtaI;

	const auto wi = refract(wo, face_forward(vector3(0, 0, 1), wo), eta_i / eta_o);

	if (wi == vector3(0)) return {};

	const auto factor = mMode == transport_mode::radiance ? (eta_i * eta_i) / (eta_o * eta_o) : 1;
	const auto value = mTransmission * (spectrum(1) - fresnel) * factor / math::abs(cos_theta(wi));

	return scattering_sample(
		scattering_type::transmission | scattering_type::specular,
		value, wi, 1 - fresnel);
}

rainbow::core::real rainbow::cpus::scatterings::fresnel_specular::pdf(const vector3& wo, const vector3& wi) const
{
	return 0;
}
