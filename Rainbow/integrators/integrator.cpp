#include "integrator.hpp"

rainbow::integrators::sampler_group::sampler_group(
	const std::shared_ptr<samplers::sampler1d>& sampler1d,
	const std::shared_ptr<samplers::sampler2d>& sampler2d) :
	sampler1d(sampler1d), sampler2d(sampler2d)
{
}

rainbow::integrators::sampler_integrator::sampler_integrator(
	const std::shared_ptr<sampler2d>& camera_sampler, size_t max_depth) :
	mMaxDepth(max_depth), mCameraSampler(camera_sampler)
{
}

void rainbow::integrators::sampler_integrator::render(
	const std::shared_ptr<camera>& camera,
	const std::shared_ptr<scene>& scene)
{
	const auto film = camera->film();
	const auto bound = film->pixels_bound();

	// loop all pixels that we will render
	for (size_t y = bound.min.y; y < bound.max.y; y++) {
		for (size_t x = bound.min.x; x < bound.max.x; x++) {
			mCameraSampler.reset();

			// loop all samples in one pixel
			for (size_t index = 0; index < mCameraSampler->count(); index++) {
				const auto sample = vector2(x, y) + mCameraSampler->sample(index);
				const auto ray = camera->generate_ray(sample);

				const auto spectrum = trace(scene, prepare_samplers(), ray, 0);

				film->add_sample(sample, spectrum);
			}
		}
	}
}

rainbow::integrators::sampler_group rainbow::integrators::sampler_integrator::prepare_samplers()
{
	return {};
}
