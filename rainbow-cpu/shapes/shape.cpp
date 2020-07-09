#include "shape.hpp"

rainbow::cpus::shapes::shape_sample::shape_sample(const interactions::interaction& interaction, real pdf) :
	interaction(interaction), pdf(pdf)
{
	
}

rainbow::cpus::shapes::shape_sample rainbow::cpus::shapes::shape_sample::transform(const shared::transform& transform, const shape_sample& sample)
{
	return shape_sample(
		transform_interaction(transform, sample.interaction),
		sample.pdf
	);
}

rainbow::cpus::shapes::shape_instance_properties::shape_instance_properties(const std::shared_ptr<const shapes::shape>& shape, real area)
	: shape(shape), area(area)
{
}

rainbow::cpus::shapes::shape::shape(bool reverse_orientation, size_t count) : mReverseOrientation(reverse_orientation), mCount(count)
{
}

rainbow::cpus::shapes::shape_sample rainbow::cpus::shapes::shape::sample(const shape_instance_properties& properties, 
	const interaction& reference, const vector2& sample) const
{
	auto shape_sample = this->sample(properties, sample);
	auto wi = shape_sample.interaction.point - reference.point;

	if (length_squared(wi) == 0) return {};

	wi = normalize(wi);

	// notice : convert the pdf from area measure to solid angle measure
	shape_sample.pdf = shape_sample.pdf *
		distance_squared(reference.point, shape_sample.interaction.point) /
		abs(dot(shape_sample.interaction.normal, -wi));

	if (isinf(shape_sample.pdf)) return {};

	return shape_sample;
}

rainbow::core::real rainbow::cpus::shapes::shape::pdf(const shape_instance_properties& properties, const interaction& reference, const vector3& wi) const
{
	const auto ray = reference.spawn_ray(wi);
	const auto interaction = intersect(ray);

	// if the ray is not intersect the shape, we return 0
	if (!interaction.has_value()) return 0;

	const auto pdf = distance_squared(reference.point, interaction->point) /
		(abs(dot(interaction->normal, -wi)) * properties.area);

	if (isinf(pdf)) return 0;

	return pdf;
}

rainbow::cpus::shapes::shape_instance_properties rainbow::cpus::shapes::shape::instance(const transform& transform) const noexcept
{
	return shape_instance_properties(shared_from_this(), area(transform));
}

bool rainbow::cpus::shapes::shape::reverse_orientation() const noexcept
{
	return mReverseOrientation;
}

size_t rainbow::cpus::shapes::shape::count() const noexcept
{
	return mCount;
}
