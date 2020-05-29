#include "directional_light.hpp"

rainbow::emitters::directional_light::directional_light(const spectrum& irradiance, const vector3& direction, real radius) :
	emitter(emitter_type::delta_direction),
	mIrradiance(irradiance), mDirection(normalize(direction)), mRadius(radius)
{
	// mIrradiance : the amount of spectral power per unit area received by light
	// mDirection : the direction of light, from point on surface to light
	// mRadius : the distance from point on surface to light(or the radius of world)
}

rainbow::spectrum rainbow::emitters::directional_light::evaluate(const interaction& interaction,
	const vector3& wi) const
{
	return 0;
}

rainbow::emitters::emitter_sample rainbow::emitters::directional_light::sample(const std::shared_ptr<shape>& shape,
	const interaction& reference, const vector2& sample) const
{
	return emitter_sample(
		mIrradiance,
		reference.point + 2 * mRadius * mDirection,
		mDirection,
		1
	);
}

rainbow::real rainbow::emitters::directional_light::pdf(const std::shared_ptr<shape>& shape,
	const interaction& reference, const vector3& wi) const
{
	return 0;
}

rainbow::spectrum rainbow::emitters::directional_light::power(const std::shared_ptr<shape>& shape) const
{
	return mIrradiance * pi<real>() * mRadius * mRadius;
}
