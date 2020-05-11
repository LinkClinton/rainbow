#include "transmission_function.hpp"

#include "../../shared/sample_function.hpp"

rainbow::scatterings::transmission_function::transmission_function(const scattering_type& type, const spectrum& transmission) :
	bidirectional_scattering_distribution_function(type | scattering_type::transmission), mTransmission(transmission)
{
}

rainbow::scatterings::scattering_sample rainbow::scatterings::transmission_function::sample(const vector3& wo, const vector2& sample) const
{
	// sample the wi in hemisphere(reflection)
	auto wi = cosine_sample_hemisphere(sample);

	// when wo.z less than 0, wi.z should less than 0, too.
	// there are in the same hemisphere
	if (wo.z > 0) wi.z = -wi.z;

	return scattering_sample(
		mType, evaluate(wo, wi), wi, pdf(wo, wi)
	);
}

rainbow::real rainbow::scatterings::transmission_function::pdf(const vector3& wo, const vector3& wi) const
{
	return same_hemisphere(wo, wi) ? 0 : cosine_sample_hemisphere_pdf(abs(cos_theta(wi)));
}