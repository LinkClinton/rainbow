#include "surface_light.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::emitters::surface_light::surface_light(const spectrum& radiance) :
	emitter(emitter_type::surface), mRadiance(radiance)
{
}

spectrum rainbow::cpus::emitters::surface_light::evaluate(const interaction& interaction, const vector3& wi) const 
{
	// if the wi and surface normal of emitter are not in the same side, the emitter can not lighting it
	return (dot(interaction.normal, wi) > 0) ? mRadiance : 0;
}

rainbow::cpus::emitters::emitter_sample rainbow::cpus::emitters::surface_light::sample(
	const std::shared_ptr<shape>& shape, const interaction& reference, const vector2& sample) const
{
	const auto shape_sample = shape->sample(reference, sample);

	if (shape_sample.pdf == 0) return {};

	const auto wi = normalize(shape_sample.interaction.point - reference.point);
	
	return emitter_sample(
		evaluate(shape_sample.interaction, -wi),
		shape_sample.interaction.point,
		wi,
		shape_sample.pdf
	);
}

rainbow::core::real rainbow::cpus::emitters::surface_light::pdf(
	const std::shared_ptr<shape>& shape, const interaction& reference, const vector3& wi) const
{
	return shape->pdf(reference, wi);
}


spectrum rainbow::cpus::emitters::surface_light::power(const std::shared_ptr<shape>& shape) const
{
	return mRadiance * shape->area() * pi<real>();
}
