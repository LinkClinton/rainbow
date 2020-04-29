#include "environment_light.hpp"

#include "../shared/sample_function.hpp"

rainbow::emitters::environment_light::environment_light(const spectrum& intensity, real radius) :
	emitter(emitter_type::environment), mIntensity(intensity), mRadius(radius)
{
}

rainbow::spectrum rainbow::emitters::environment_light::evaluate(
	const interaction& interaction, const vector3& wi) const
{
	// in environment light, the interaction is always with default interaction.
	// the wi will be the inverse direction of the trace ray
	// in current version, we only return the diffuse intensity
	return mIntensity;
}

rainbow::emitters::emitter_sample rainbow::emitters::environment_light::sample(
	const std::shared_ptr<shape>& shape, const interaction& reference, const vector2& sample) const
{
	// in current version, we only sample it on sphere
	const auto position = vector3(0) + mRadius * uniform_sample_sphere(sample);
	const auto wi = normalize(position - reference.point);
	
	return emitter_sample(
		mIntensity,
		position,
		wi,
		pdf(shape, reference, wi)
	);
}

rainbow::real rainbow::emitters::environment_light::pdf(
	const std::shared_ptr<shape>& shape, const interaction& reference, const vector3& wi) const
{
	// in current version, we sample it on sphere, so the pdf is the 1 / area
	return 1 / (4 * mRadius * mRadius * pi<real>());
}

rainbow::spectrum rainbow::emitters::environment_light::power(const std::shared_ptr<shape>& shape) const
{
	return mIntensity * mRadius * mRadius * pi<real>();
}
