#include "specular_transmission.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::scatterings::specular_transmission::specular_transmission(
	const spectrum& transmission, real eta_i, real eta_o) :
	transmission_function(scattering_type::specular, transmission),
	mFresnel(std::make_shared<fresnel_effect_dielectric>(eta_i, eta_o)),
	mEtaI(eta_i), mEtaO(eta_o)
{
}

spectrum rainbow::cpus::scatterings::specular_transmission::evaluate(const vector3& wo, const vector3& wi) const
{
	return 0;
}

rainbow::cpus::scatterings::scattering_sample rainbow::cpus::scatterings::specular_transmission::sample(
	const vector3& wo, const vector2& sample) const
{
	// the surface normal indicate the outside of surface
	// when dot(wo, normal) > 0, it means the ray intersect point on the surface from outside
	// so the eta_i should be mEtaI(the eta of medium where surface normal on)
	const auto entering = cos_theta(wo) > 0;

	const auto eta_i = entering ? mEtaI : mEtaO;
	const auto eta_o = entering ? mEtaO : mEtaI;

	// the normal should be the same hemisphere with wo to compute the wi
	const auto wi = refract(wo, face_forward(vector3(0, 0, 1), wo), eta_i / eta_o);

	if (wi == vector3(0)) return {};

	// if entering == true, the cos_theta(wi) should less than 0, so the eta_i and eta_o will be swapped in fresnel
	// if entering == false, the cos_theta(wi) should greater than 0
	const auto fresnel = mFresnel->evaluate(cos_theta(wi));
	const auto factor = (eta_i * eta_i) / (eta_o * eta_o);
	const auto value = mTransmission * (spectrum(1) - fresnel) * factor / math::abs(cos_theta(wi));
	
	return scattering_sample(
		mType,
		value,
		wi,
		1
	);
}

rainbow::core::real rainbow::cpus::scatterings::specular_transmission::pdf(const vector3& wo, const vector3& wi) const
{
	return 0;
}
