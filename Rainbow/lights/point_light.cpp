#include "point_light.hpp"

rainbow::lights::point_light::point_light(const transform& transform, const spectrum& irradiance) :
	light(transform, light_type::delta_position),
	mIrradiance(irradiance), mPosition(transform(vector3(0)))
{
}

rainbow::lights::light_sample rainbow::lights::point_light::sample(const vector3& point, const vector2& sample)
{
	return light_sample(
		mIrradiance / distance_squared(mPosition, point),
		normalize(mPosition - point),
		1
	);
}

rainbow::real rainbow::lights::point_light::pdf(const vector3& point, const vector3& wi)
{
	return 0;
}

rainbow::spectrum rainbow::lights::point_light::power()
{
	return mIrradiance * 4 * pi<real>();
}
