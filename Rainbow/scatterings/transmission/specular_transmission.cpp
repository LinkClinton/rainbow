#include "specular_transmission.hpp"

rainbow::scatterings::specular_transmission::specular_transmission(
	const spectrum& transmission, real eta_i, real eta_o) :
	transmission_function(scattering_type::specular, transmission),
	mFresnel(std::make_shared<fresnel_effect_dielectric>(eta_i, eta_o)),
	mEtaI(eta_i), mEtaO(eta_o)
{
}

rainbow::spectrum rainbow::scatterings::specular_transmission::evaluate(const vector3& wo, const vector3& wi) const
{
	return 0;
}

rainbow::scatterings::scattering_sample rainbow::scatterings::specular_transmission::sample(
	const vector3& wo, const vector2& sample) const
{
	// the surface normal in the internal space, wo when wo and normal are in the same hemisphere
	// the wi should from the out world.
	const auto entering = cos_theta(wo) > 0;

	const auto eta_i = entering ? mEtaI : mEtaO;
	const auto eta_o = entering ? mEtaO : mEtaI;

	// the normal should be the same hemisphere with wo to compute the wi
	const auto wi = refract(wo, face_forward(vector3(0, 0, 1), wo), eta_i / eta_o);

	if (wi == vector3(0)) return {};

	// if entering == true, the cos_theta(wi) should less than 0, so the eta_i and eta_o will be swapped in fresnel
	// if entering == false, the cos_theta(wi) should greater than 0
	const auto fresnel = mFresnel->evaluate(cos_theta(wi));
	const auto value = mTransmission * (spectrum(1) - fresnel) / abs(cos_theta(wi));
	
	return scattering_sample(
		mType,
		value,
		wi,
		1
	);
}

rainbow::real rainbow::scatterings::specular_transmission::pdf(const vector3& wo, const vector3& wi) const
{
	return 0;
}
