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
	const auto local_ray = mWorldToLocal(ray);
	const auto [t0, t1, ret] = intersect(bound3(vector3(0), vector3(1)), local_ray);

	if (ret == false) return spectrum(1);

	real value = 1, t = t0;

	while (true) {
		t = t - log(1 - sampler->next().x) * mInvMaxDensity;

		if (t >= t1) break;

		const auto position = local_ray.origin + local_ray.direction * t;
		const auto sigma_t = mSigmaT->sample(position);
		
		value = value * (1 - max(static_cast<real>(0), sigma_t * mInvMaxDensity));
	}

	return spectrum(value);
}

rainbow::cpus::media::medium_sample rainbow::cpus::media::heterogeneous_medium::sample(
	const std::shared_ptr<sampler1d>& sampler, const ray& ray) const
{
	const auto local_ray = mWorldToLocal(ray);
	const auto [t0, t1, ret] = intersect(bound3(vector3(0), vector3(1)), local_ray);

	if (ret == false) return medium_sample(std::nullopt, spectrum(1));

	auto t = t0;

	while (true) {
		t = t - log(1 - sampler->next().x) * mInvMaxDensity;

		if (t >= t1) break;

		const auto position = local_ray.origin + local_ray.direction * t;
		const auto sigma_t = mSigmaT->sample(position);

		if (sigma_t * mInvMaxDensity > sampler->next().x) {
			const auto albedo = mAlbedo->sample(position);
			const auto g = mG->sample(position);

			return medium_sample(
				medium_interaction(
					std::make_shared<henyey_greenstein>(g),
					ray.origin + ray.direction * t,
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
