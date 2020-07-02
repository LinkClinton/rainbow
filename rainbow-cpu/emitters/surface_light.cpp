#include "surface_light.hpp"

#include "../../rainbow-core/sample_function.hpp"

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

rainbow::cpus::emitters::emitter_ray_sample rainbow::cpus::emitters::surface_light::sample(
	const std::shared_ptr<shape>& shape, const vector2& sample0, const vector2& sample1) const
{
	// first, sample the shape to find the origin of the ray
	// second, sample the hemisphere to find the direction of the ray
	const auto shape_sample = shape->sample(sample0);
	const auto direction = cosine_sample_hemisphere(sample1);

	// transform the direction from sample space to local space and build the ray
	const auto system = coordinate_system(shape_sample.interaction.normal);
	const auto ray = shape_sample.interaction.spawn_ray(local_to_world(system, direction));

	return emitter_ray_sample(
		evaluate(shape_sample.interaction, ray.direction),
		shape_sample.interaction.normal,
		ray,
		cosine_sample_hemisphere_pdf(direction.z),
		shape_sample.pdf
	);
}

rainbow::cpus::emitters::emitter_sample rainbow::cpus::emitters::surface_light::sample(
	const std::shared_ptr<shape>& shape, const interaction& reference, const vector2& sample) const
{
	const auto shape_sample = shape->sample(reference, sample);

	if (shape_sample.pdf == 0) return {};

	const auto wi = normalize(shape_sample.interaction.point - reference.point);
	
	return emitter_sample(
		shape_sample.interaction,
		evaluate(shape_sample.interaction, -wi),
		wi,
		shape_sample.pdf
	);
}

std::tuple<real, real> rainbow::cpus::emitters::surface_light::pdf(const std::shared_ptr<shape>& shape, const ray& ray,
	const vector3& normal) const
{
	// [pdf_position, pdf_direction]
	return { shape->pdf(), cosine_sample_hemisphere_pdf(dot(normal, ray.direction)) };
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
