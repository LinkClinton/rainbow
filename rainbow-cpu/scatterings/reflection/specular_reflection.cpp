#include "specular_reflection.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::scatterings::specular_reflection::specular_reflection(
	const std::shared_ptr<fresnel_effect>& fresnel, const spectrum& reflectance) :
	reflection_function(scattering_type::specular, reflectance), mFresnel(fresnel)
{
}

spectrum rainbow::cpus::scatterings::specular_reflection::evaluate(const vector3& wo, const vector3& wi) const
{
	return 0;
}

rainbow::cpus::scatterings::scattering_sample rainbow::cpus::scatterings::specular_reflection::sample(
	const vector3& wo, const vector2& sample) const
{
	// the wi should be (-wo.x, -wo.y, wo.z) in reflection space(shading space)
	// because the normal is (0, 0, 1)
	const auto wi = vector3(-wo.x, -wo.y, wo.z);

	// the value of function is f(wo, wi) = Fr(wi) / |cos_theta(wi)|
	// specular reflection is delta distribution, so the wi should be reflect(wi, n)
	// other vector is 0
	return scattering_sample(
		mType,
		mFresnel->evaluate(cos_theta(wi)) * mReflectance / math::abs(cos_theta(wi)),
		wi,
		1
	);
}

rainbow::core::real rainbow::cpus::scatterings::specular_reflection::pdf(const vector3& wo, const vector3& wi) const
{
	return 0;
}
