#include "point_light.hpp"

rainbow::emitters::point_light::point_light(const spectrum& intensity) :
	emitter(emitter_type::delta_position),
	mIntensity(intensity)
{
}

rainbow::spectrum rainbow::emitters::point_light::evaluate(const interaction& interaction, const vector3& wi) const
{
	// point light is delta position light, so can not evaluate it
	return 0;
}

rainbow::emitters::emitter_sample rainbow::emitters::point_light::sample(
	const std::shared_ptr<shape>& shape, const interaction& reference, const vector2& sample) const
{
	const auto position = vector3(0);
	
	return emitter_sample(
		mIntensity / distance_squared(position, reference.point),
		position,
		normalize(position - reference.point),
		1
	);
}

rainbow::real rainbow::emitters::point_light::pdf(
	const std::shared_ptr<shape>& shape, const interaction& reference, const vector3& wi) const
{
	return 0;
}

rainbow::spectrum rainbow::emitters::point_light::power(const std::shared_ptr<shape>& shape) const
{
	return mIntensity * 4 * pi<real>();
}
