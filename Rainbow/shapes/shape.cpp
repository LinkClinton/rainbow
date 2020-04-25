#include "shape.hpp"

rainbow::shapes::shape_sample::shape_sample(const interactions::interaction& interaction, real pdf) :
	interaction(interaction), pdf(pdf)
{
	
}

rainbow::shapes::shape_sample rainbow::shapes::shape_sample::transform(const rainbow::transform& transform, const shape_sample& sample)
{
	return shape_sample(
		transform_interaction(transform, sample.interaction),
		sample.pdf
	);
}

rainbow::shapes::shape::shape(bool reverse_orientation) : mReverseOrientation(reverse_orientation)
{
}

rainbow::shapes::shape_sample rainbow::shapes::shape::sample(const interaction& reference, const vector2& sample) const
{
	auto shape_sample = this->sample(sample);
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

rainbow::real rainbow::shapes::shape::pdf(const interaction& reference, const vector3& wi) const
{
	const auto ray = reference.spawn_ray(wi);
	const auto interaction = intersect(ray);

	// if the ray is not intersect the shape, we return 0
	if (!interaction.has_value()) return 0;

	const auto pdf = distance_squared(reference.point, interaction->point) /
		(abs(dot(interaction->normal, -wi)) * area());

	if (isinf(pdf)) return 0;

	return pdf;
}

bool rainbow::shapes::shape::reverse_orientation() const noexcept
{
	return mReverseOrientation;
}
