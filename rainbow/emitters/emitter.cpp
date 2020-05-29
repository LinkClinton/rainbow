#include "emitter.hpp"

rainbow::emitters::emitter_type rainbow::emitters::operator&(const emitter_type& lhs, const emitter_type& rhs)
{
	return static_cast<emitter_type>(static_cast<uint32>(lhs) & static_cast<uint32>(rhs));
}

bool rainbow::emitters::has(const emitter_type& type, const emitter_type& flag)
{
	return (type & flag) == flag;
}

rainbow::emitters::emitter_sample::emitter_sample(const spectrum& intensity, const vector3& position, const vector3& wi, real pdf) :
	intensity(intensity), position(position), wi(wi), pdf(pdf)
{
}

rainbow::emitters::emitter_sample rainbow::emitters::emitter_sample::transform(const rainbow::transform& transform, const emitter_sample& sample)
{
	return emitter_sample(
		sample.intensity,
		transform_point(transform, sample.position),
		transform_vector(transform, sample.wi),
		sample.pdf
	);
}

bool rainbow::emitters::is_delta_emitter(const emitter_type& type)
{
	return has(type, emitter_type::delta_position) | has(type, emitter_type::delta_direction);
}

bool rainbow::emitters::is_environment_emitter(const emitter_type& type)
{
	return has(type, emitter_type::environment);
}

rainbow::emitters::emitter::emitter(const emitter_type& type) : mType(type)
{
}

rainbow::emitters::emitter_type rainbow::emitters::emitter::type() const noexcept
{
	return mType;
}

bool rainbow::emitters::emitter::is_environment() const noexcept
{
	return is_environment_emitter(mType);
}

bool rainbow::emitters::emitter::is_delta() const noexcept
{
	return is_delta_emitter(mType);
}
