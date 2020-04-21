#include "point_light.hpp"

rainbow::emitters::point_light::point_light(const transform& transform, const spectrum& irradiance) :
	emitter(transform, emitter_type::delta_position),
	mIrradiance(irradiance), mPosition(transform_point(transform, vector3(0)))
{
}

rainbow::emitters::emitter_sample rainbow::emitters::point_light::sample(const vector3& point, const vector2& sample)
{
	return emitter_sample(
		mIrradiance / distance_squared(mPosition, point),
		mPosition,
		normalize(mPosition - point),
		1
	);
}

rainbow::real rainbow::emitters::point_light::pdf(const vector3& point, const vector3& wi)
{
	return 0;
}

rainbow::spectrum rainbow::emitters::point_light::power()
{
	return mIrradiance * 4 * pi<real>();
}
