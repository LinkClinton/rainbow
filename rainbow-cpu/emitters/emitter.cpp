#include "emitter.hpp"

rainbow::cpus::emitters::emitter_type rainbow::cpus::emitters::operator&(const emitter_type& lhs, const emitter_type& rhs)
{
	return static_cast<emitter_type>(static_cast<uint32>(lhs) & static_cast<uint32>(rhs));
}

bool rainbow::cpus::emitters::has(const emitter_type& type, const emitter_type& flag)
{
	return (type & flag) == flag;
}

rainbow::cpus::emitters::emitter_sample::emitter_sample(const spectrum& intensity, const vector3& position, const vector3& wi, real pdf) :
	intensity(intensity), position(position), wi(wi), pdf(pdf)
{
}

rainbow::cpus::emitters::emitter_sample rainbow::cpus::emitters::emitter_sample::transform(const shared::transform& transform, const emitter_sample& sample)
{
	return emitter_sample(
		sample.intensity,
		transform_point(transform, sample.position),
		transform_vector(transform, sample.wi),
		sample.pdf
	);
}

rainbow::cpus::emitters::emitter_ray_sample::emitter_ray_sample(
	const spectrum& intensity, const vector3& normal, const shared::ray& ray, real pdf_direction, real pdf_position) :
	intensity(intensity), normal(normal), ray(ray), pdf_direction(pdf_direction), pdf_position(pdf_position)
{
}

rainbow::cpus::emitters::emitter_ray_sample rainbow::cpus::emitters::emitter_ray_sample::transform(
	const shared::transform& transform, const emitter_ray_sample& sample)
{
	return emitter_ray_sample(
		sample.intensity,
		transform_normal(transform, sample.normal),
		transform(sample.ray),
		sample.pdf_direction, sample.pdf_position);
}

bool rainbow::cpus::emitters::is_delta_emitter(const emitter_type& type)
{
	return has(type, emitter_type::delta_position) | has(type, emitter_type::delta_direction);
}

bool rainbow::cpus::emitters::is_environment_emitter(const emitter_type& type)
{
	return has(type, emitter_type::environment);
}

rainbow::cpus::emitters::emitter::emitter(const emitter_type& type) : mType(type)
{
}

rainbow::cpus::emitters::emitter_type rainbow::cpus::emitters::emitter::type() const noexcept
{
	return mType;
}

bool rainbow::cpus::emitters::emitter::is_environment() const noexcept
{
	return is_environment_emitter(mType);
}

bool rainbow::cpus::emitters::emitter::is_delta() const noexcept
{
	return is_delta_emitter(mType);
}
