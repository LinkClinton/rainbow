#include "point_light.hpp"

#include "../../rainbow-core/sample_function.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::emitters::point_light::point_light(const spectrum& intensity) :
	emitter(emitter_type::delta_position),
	mIntensity(intensity)
{
}

spectrum rainbow::cpus::emitters::point_light::evaluate(const interaction& interaction, const vector3& wi) const
{
	// point light is delta position light, so can not evaluate it
	return 0;
}

rainbow::cpus::emitters::emitter_ray_sample rainbow::cpus::emitters::point_light::sample(
	const shape_instance_properties& properties, const vector2& sample0, const vector2& sample1) const
{
	// sample the direction of ray spawn to 
	const auto direction = uniform_sample_sphere(sample0);
	
	return emitter_ray_sample(
		mIntensity,
		direction,
		ray(direction, vector3(0)),
		uniform_sample_sphere_pdf(),
		1
	);
}

rainbow::cpus::emitters::emitter_sample rainbow::cpus::emitters::point_light::sample(
	const shape_instance_properties& properties, const interaction& reference, const vector2& sample) const
{
	const auto position = vector3(0);
	
	return emitter_sample(
		interaction(position),
		mIntensity / distance_squared(position, reference.point),
		normalize(position - reference.point),
		1
	);
}

std::tuple<real, real> rainbow::cpus::emitters::point_light::pdf(
	const shape_instance_properties& properties, const ray& ray, const vector3& normal) const
{
	// [pdf_position, pdf_direction]
	return { static_cast<real>(1), uniform_sample_sphere_pdf() };
}

rainbow::core::real rainbow::cpus::emitters::point_light::pdf(
	const shape_instance_properties& properties, const interaction& reference, const vector3& wi) const
{
	return 0;
}

spectrum rainbow::cpus::emitters::point_light::power(const shape_instance_properties& properties) const
{
	return mIntensity * 4 * pi<real>();
}
