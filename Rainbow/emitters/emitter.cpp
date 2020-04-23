#include "emitter.hpp"

rainbow::emitters::emitter_sample::emitter_sample(const spectrum& irradiance, const vector3& position, const vector3& wi, real pdf) :
	intensity(irradiance), position(position), wi(wi), pdf(pdf)
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
	return type == emitter_type::delta_position;
}

rainbow::emitters::emitter::emitter(const emitter_type& type) : mType(type)
{
}

rainbow::emitters::emitter_type rainbow::emitters::emitter::type() const noexcept
{
	return mType;
}

bool rainbow::emitters::emitter::is_delta() const noexcept
{
	return is_delta_emitter(mType);
}
