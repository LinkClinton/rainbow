#include "emitter.hpp"

rainbow::emitters::emitter_sample::emitter_sample(const spectrum& irradiance, const vector3& position, const vector3& wi, real pdf) :
	irradiance(irradiance), position(position), wi(wi), pdf(pdf)
{
}

rainbow::emitters::emitter_sample rainbow::emitters::emitter_sample::transform(const rainbow::transform& transform, const emitter_sample& sample)
{
	return emitter_sample(
		sample.irradiance,
		transform_point(transform, sample.position),
		transform_vector(transform, sample.wi),
		sample.pdf
	);
}

rainbow::emitters::emitter::emitter(const transform& transform, const emitter_type& type) :
	mEmitterToWorld(transform), mWorldToEmitter(transform.inverse()), mType(type)
{
	
}

rainbow::emitters::emitter_type rainbow::emitters::emitter::type() const noexcept
{
	return mType;
}
