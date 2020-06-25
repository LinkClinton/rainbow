#include "lambertian_transmission.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::scatterings::lambertian_transmission::lambertian_transmission(
	const transport_mode& mode, const spectrum& transmission) :
	transmission_function(scattering_type::diffuse, mode, transmission)
{
}

spectrum rainbow::cpus::scatterings::lambertian_transmission::evaluate(const vector3& wo, const vector3& wi) const
{
	return mTransmission * one_over_pi<real>();
}
