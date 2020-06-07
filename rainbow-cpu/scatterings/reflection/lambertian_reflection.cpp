#include "lambertian_reflection.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::scatterings::lambertian_reflection::lambertian_reflection(const spectrum& reflectance) :
	reflection_function(scattering_type::diffuse, reflectance)
{
}

spectrum rainbow::cpus::scatterings::lambertian_reflection::evaluate(const vector3& wo, const vector3& wi) const
{
	return mReflectance * one_over_pi<real>();
}
