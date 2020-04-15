#include "integrator.hpp"

#define __PARALLEL_RENDER__

#include <execution>

rainbow::integrators::integrator_debug_info::integrator_debug_info(const vector2i& pixel) :
	pixel(pixel)
{
}

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

#ifdef __PARALLEL_RENDER__
	
	struct parallel_input {
		vector2 position;

		size_t index;
	};

	struct parallel_output {
		spectrum value;
	};

	const auto bound_size = vector2i(
		bound.max.x - bound.min.x,
		bound.max.y - bound.min.y);

	const auto sample_count =
		static_cast<size_t>(bound_size.x) *
		static_cast<size_t>(bound_size.y) *
		mCameraSampler->count();
	
	auto outputs = std::vector<parallel_output>(sample_count);
	auto inputs = std::vector<parallel_input>(sample_count);

	// loop all pixels that we will render to build samples
	// the samples we will use std::for_each to get the value of sample parallel
	for (size_t y = bound.min.y; y < bound.max.y; y++) {
		for (size_t x = bound.min.x; x < bound.max.x; x++) {
			mCameraSampler->reset();

			// loop all samples in one pixel
			for (size_t index = 0; index < mCameraSampler->count(); index++) {
				const auto sample = vector2(x, y) + mCameraSampler->sample(index);
				const auto sample_index = ((y - bound.min.y) * bound_size.x + (x - bound.min.x)) * mCameraSampler->count() + index;

				inputs[sample_index] = {
					sample,
					sample_index
				};
			}
		}
	}

	// trace the ray with samples, the prepare_samplers() should be independent per sample
	// all samples in samplers should build before rendering
	std::for_each(std::execution::par, inputs.begin(), inputs.end(), [&](const parallel_input& input)
		{
			const auto debug = integrator_debug_info(
				vector2i(input.position.x, input.position.y)
			);
		
			outputs[input.index] = {
				trace(scene, debug, prepare_samplers(), camera->generate_ray(input.position), 0)
			};
		});

	// merge the samples into film
	for (size_t index = 0; index < sample_count; index++) 
		film->add_sample(inputs[index].position, outputs[index].value);
	
#else

	for (size_t y = bound.min.y; y < bound.max.y; y++) {
		for (size_t x = bound.min.x; x < bound.max.x; x++) {
			mCameraSampler->reset();
			
			// loop all samples in one pixel
			for (size_t index = 0; index < mCameraSampler->count(); index++) {
				const auto debug = integrator_debug_info(
					vector2i(x, y)
				);

				const auto sample = vector2(x, y) + mCameraSampler->sample(index);
				const auto spectrum = trace(scene, debug, prepare_samplers(), camera->generate_ray(sample), 0);
				
				film->add_sample(sample, spectrum);
			}
		}
	}
	
#endif
}

rainbow::integrators::sampler_group rainbow::integrators::sampler_integrator::prepare_samplers()
{
	return {};
}
