#include "light.hpp"

rainbow::lights::light_sample::light_sample(const spectrum& irradiance, const vector3& position, const vector3& wi, real pdf) :
	irradiance(irradiance), position(position), wi(wi), pdf(pdf)
{
}

rainbow::lights::light::light(const transform& transform, const light_type& type) :
	mLightToWorld(transform), mWorldToLight(transform.inverse()), mType(type)
{
	
}

rainbow::lights::light_type rainbow::lights::light::type() const noexcept
{
	return mType;
}
