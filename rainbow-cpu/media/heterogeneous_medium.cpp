#include "heterogeneous_medium.hpp"

#include "../shared/phases/henyey_greenstein.hpp"
#include "../textures/image_texture.hpp"

rainbow::cpus::media::heterogeneous_medium::heterogeneous_medium(
	const std::shared_ptr<textures::texture3d<spectrum>>& albedo,
	const std::shared_ptr<textures::texture3d<real>>& sigma_t, 
	const std::shared_ptr<textures::texture3d<real>>& g,
	const transform& transform) :
	mAlbedo(albedo), mSigmaT(sigma_t), mG(g), mLocalToWorld(transform), mWorldToLocal(transform.inverse())
{
	// sigma_t should be the image_texture3d
	const auto image_sigma_t = std::static_pointer_cast<textures::image_texture3d<real>>(sigma_t);

	assert(image_sigma_t != nullptr);

	mMaxDensity = 0;

	for (size_t index = 0; index < image_sigma_t->length(); index++) 
		mMaxDensity = max(mMaxDensity, image_sigma_t->value(index));

	mInvMaxDensity = 1 / mMaxDensity;
}

rainbow::cpus::shared::spectrums::spectrum rainbow::cpus::media::heterogeneous_medium::evaluate(
	const std::shared_ptr<sampler1d>& sampler, const ray& ray) const
{
	shared::ray local_ray;

	// when mWorldToLocal has scale transform, the scale property is missed
	// because the transform::operator() will normalize the direction of ray
	// to avoid this situation, we transform the ray but do not normalize the direction
	// in fact, if you normalize the direction of ray, the segment of t is not right(without scaling)
	local_ray.length = ray.length;
	local_ray.direction = transform_vector(mWorldToLocal, ray.direction);
	local_ray.origin = transform_point(mWorldToLocal, ray.origin);

	// find the range ray intersect the medium box
	const auto [t0, t1, ret] = intersect(bound3(vector3(0), vector3(1)), local_ray);

	// if the ray is not intersect with medium, just sample the surface interaction
	if (ret == false) return spectrum(1);

	real value = 1, t = t0;

	while (true) {
		t = t - log(1 - sampler->next().x) * mInvMaxDensity;

		// out of range
		if (t >= t1) break;

		const auto position = local_ray.origin + local_ray.direction * t;
		const auto sigma_t = mSigmaT->sample(position);

		//ratio tracing for estimators of transmittance
		//see https://cs.dartmouth.edu/~wjarosz/publications/novak14residual.pdf
		value = value * (1 - max(static_cast<real>(0), sigma_t * mInvMaxDensity));
	}

	return spectrum(value);
}

rainbow::cpus::media::medium_sample rainbow::cpus::media::heterogeneous_medium::sample(
	const std::shared_ptr<sampler1d>& sampler, const ray& ray) const
{
	shared::ray local_ray;

	// when mWorldToLocal has scale transform, the scale property is missed
	// because the transform::operator() will normalize the direction of ray
	// to avoid this situation, we transform the ray but do not normalize the direction
	// in fact, if you normalize the direction of ray, the segment of t is not right(without scaling)
	// you also can transform the box from local to world and then transform the position from world to space every tracing loop
	// but it damage the performance(you need do the transform every times)
	local_ray.length = ray.length;
	local_ray.direction = transform_vector(mWorldToLocal, ray.direction);
	local_ray.origin = transform_point(mWorldToLocal, ray.origin);

	// find the range ray intersect the medium box
	const auto [t0, t1, ret] = intersect(bound3(vector3(0), vector3(1)), local_ray);

	// if the ray is not intersect with medium, just sample the surface interaction
	if (ret == false) return medium_sample(std::nullopt, spectrum(1));

	auto t = t0;

	while (true) {
		// t = t - log(1 - sampler->next().x) / max_sigma_t for delta tracing
		t = t - log(1 - sampler->next().x) * mInvMaxDensity;

		// out of range
		if (t >= t1) break;

		// compute the position of current t in local space and sample sigma_t using it
		const auto position = local_ray.origin + local_ray.direction * t;
		const auto sigma_t = mSigmaT->sample(position);

		// test if it is real particle, if it is, we will sample it
		// and create it with medium_interaction
		if (sigma_t * mInvMaxDensity > sampler->next().x) {
			const auto albedo = mAlbedo->sample(position);
			const auto g = mG->sample(position);

			return medium_sample(
				medium_interaction(
					std::make_shared<henyey_greenstein>(g),
					transform_point(mLocalToWorld, position),
					-ray.direction),
				albedo
			);
		}
	}

	return medium_sample(std::nullopt, spectrum(1));
}

std::tuple<real, real, bool> rainbow::cpus::media::heterogeneous_medium::intersect(const bound3& box, const ray& ray) const
{
	auto t0 = static_cast<real>(0), t1 = ray.length;

	// enum the slab of axis-aligned bounding box
	for (int dimension = 0; dimension < 3; dimension++) {
		// if the direction[dimension] is 0, the inv_direction will be INF.
		const auto inv_direction = 1 / ray.direction[dimension];

		// find the t value of intersect point on the ray
		auto near = (box.min[dimension] - ray.origin[dimension]) * inv_direction;
		auto far = (box.max[dimension] - ray.origin[dimension]) * inv_direction;

		// if the direction of ray is negative, the near will greater than far, we need swap them.
		if (near > far) std::swap(near, far);

		t0 = math::max(t0, near);
		t1 = math::min(t1, far);

		if (t0 > t1) return { static_cast<real>(0), static_cast<real>(0), false };
	}

	return { t0, t1, true };
}
