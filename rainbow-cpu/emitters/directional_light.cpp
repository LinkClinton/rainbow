#include "directional_light.hpp"

#include "../../rainbow-core/sample_function.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::emitters::directional_light::directional_light(const spectrum& irradiance, const vector3& direction, real radius) :
	emitter(emitter_type::delta_direction),
	mIrradiance(irradiance), mDirection(normalize(direction)), mRadius(radius)
{
	// mIrradiance : the amount of spectral power per unit area received by light
	// mDirection : the direction of light, from point on surface to light
	// mRadius : the distance from point on surface to light(or the radius of world)
}

spectrum rainbow::cpus::emitters::directional_light::evaluate(const interaction& interaction,
	const vector3& wi) const
{
	return 0;
}

rainbow::cpus::emitters::emitter_ray_sample rainbow::cpus::emitters::directional_light::sample(
	const shape_instance_properties& properties, const vector2& sample0, const vector2& sample1) const
{
	// first, build a system with z = direction and sample a point on disk
	// transform the point on disk to local space
	// second, transform the point with vector(direction * radius)
	// we will use this point as the origin of ray
	const auto system = coordinate_system(mDirection);
	const auto point_in_disk = concentric_sample_disk(sample0);
	const auto point_in_local = mRadius * (point_in_disk.x * system.x() + point_in_disk.y * system.y());

	return emitter_ray_sample(
		mIrradiance,
		-mDirection,
		ray(-mDirection, point_in_local + mRadius * mDirection),
		1, 1 / (pi<real>() * mRadius * mRadius)
	);
}

rainbow::cpus::emitters::emitter_sample rainbow::cpus::emitters::directional_light::sample(
	const shape_instance_properties& properties, const interaction& reference, const vector2& sample) const
{
	return emitter_sample(
		interaction(reference.point + 2 * mRadius * mDirection),
		mIrradiance,
		mDirection,
		1
	);
}

std::tuple<real, real> rainbow::cpus::emitters::directional_light::pdf(
	const shape_instance_properties& properties,  const ray& ray, const vector3& normal) const
{
	// [pdf_position, pdf_direction]
	return { 1 / (pi<real>() * mRadius * mRadius), static_cast<real>(1) };
}

rainbow::core::real rainbow::cpus::emitters::directional_light::pdf(
	const shape_instance_properties& properties, const interaction& reference, const vector3& wi) const
{
	return 0;
}

spectrum rainbow::cpus::emitters::directional_light::power(const shape_instance_properties& properties) const
{
	return mIrradiance * pi<real>() * mRadius * mRadius;
}
