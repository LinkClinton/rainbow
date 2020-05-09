#include "lambertian_transmission.hpp"

rainbow::scatterings::lambertian_transmission::lambertian_transmission(const spectrum& transmission) :
	transmission_function(scattering_type::diffuse, transmission)
{
}

rainbow::spectrum rainbow::scatterings::lambertian_transmission::evaluate(const vector3& wo, const vector3& wi) const
{
	return mTransmission * one_over_pi<real>();
}
