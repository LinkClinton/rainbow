#include "microfacet_distribution.hpp"

#include "../scattering_function.hpp"

rainbow::cpus::scatterings::microfacet_distribution::microfacet_distribution(bool sample_visible_area) :
	mSampleVisibleArea(sample_visible_area)
{
}

rainbow::core::real rainbow::cpus::scatterings::microfacet_distribution::masking_shadowing(const vector3& w) const
{
	// G = 1 / (1 + lambda(w)).
	return 1 / (1 + lambda(w));
}

rainbow::core::real rainbow::cpus::scatterings::microfacet_distribution::masking_shadowing(
	const vector3& wo, const vector3& wi) const
{
	// G = G(wo) * G(wi), but we use G = 1 / (1 + lambda(wo) + lambda(wi)).
	return 1 / (1 + lambda(wo) + lambda(wi));
}

rainbow::core::real rainbow::cpus::scatterings::microfacet_distribution::pdf(const vector3& wo, const vector3& wh) const
{
	// mSampleVisibleArea means we will only sample the microfacet we can see.
	if (mSampleVisibleArea)
		return distribution(wh) * masking_shadowing(wo) * math::abs(math::dot(wo, wh)) / math::abs(cos_theta(wo));
	else
		return distribution(wh) * math::abs(cos_theta(wh));
}
