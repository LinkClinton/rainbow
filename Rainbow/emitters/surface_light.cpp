#include "surface_light.hpp"

rainbow::emitters::surface_light::surface_light(const std::shared_ptr<shape>& surface, const spectrum& radiance) :
	emitter(surface->transform(), emitter_type::surface),
	mSurface(surface), mRadiance(radiance), mSurfaceArea(surface->area())
{
}

rainbow::spectrum rainbow::emitters::surface_light::evaluate(const interaction& interaction, const vector3& wi) const noexcept
{
	// if the wi and surface normal of emitter are not in the same side, the emitter can not lighting it
	return (dot(interaction.normal, wi) > 0) ? mRadiance : 0;
}

std::shared_ptr<rainbow::shape> rainbow::emitters::surface_light::surface() const noexcept
{
	return mSurface;
}

rainbow::emitters::emitter_sample rainbow::emitters::surface_light::sample(const interaction& reference, const vector2& sample)
{
	const auto shape_sample = mSurface->sample(reference, sample);

	if (shape_sample.pdf == 0) return {};

	const auto wi = normalize(shape_sample.interaction.point - reference.point);
	
	return emitter_sample(
		evaluate(shape_sample.interaction, -wi),
		shape_sample.interaction.point,
		wi,
		shape_sample.pdf
	);
}

rainbow::real rainbow::emitters::surface_light::pdf(const interaction& reference, const vector3& wi)
{
	return mSurface->pdf(reference, wi);
}

rainbow::spectrum rainbow::emitters::surface_light::power()
{
	return mRadiance * mSurfaceArea * pi<real>();
}
