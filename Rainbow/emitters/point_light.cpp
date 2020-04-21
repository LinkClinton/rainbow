#include "point_light.hpp"

rainbow::emitters::point_light::point_light(const transform& transform, const spectrum& intensity) :
	emitter(transform, emitter_type::delta_position),
	mIntensity(intensity), mPosition(transform_point(transform, vector3(0)))
{
}

rainbow::emitters::emitter_sample rainbow::emitters::point_light::sample(const interaction& reference, const vector2& sample)
{
	return emitter_sample(
		mIntensity / distance_squared(mPosition, reference.point),
		mPosition,
		normalize(mPosition - reference.point),
		1
	);
}

rainbow::real rainbow::emitters::point_light::pdf(const interaction& reference, const vector3& wi)
{
	return 0;
}

rainbow::spectrum rainbow::emitters::point_light::power()
{
	return mIntensity * 4 * pi<real>();
}
