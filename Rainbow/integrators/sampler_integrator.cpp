#include "sampler_integrator.hpp"

#define __PARALLEL_RENDER__

#include <execution>

rainbow::integrators::sampler_integrator::sampler_integrator(
	const std::shared_ptr<sampler2d>& sampler2d, size_t max_depth) :
	mSampler2D(sampler2d), mMaxDepth(max_depth)
{
}

void rainbow::integrators::sampler_integrator::render(
	const std::shared_ptr<camera>& camera,
	const std::shared_ptr<scene>& scene)
{
	const auto film = camera->film();
	const auto bound = film->pixels_bound();

	const auto bound_size = vector2i(
		bound.max.x - bound.min.x,
		bound.max.y - bound.min.y);

	const auto tile_size = static_cast<size_t>(16);
	const auto tile_count = vector2i(
		(bound_size.x + tile_size - 1) / tile_size,
		(bound_size.y + tile_size - 1) / tile_size);

	const auto sample_count =
		static_cast<size_t>(bound_size.x) *
		static_cast<size_t>(bound_size.y) *
		mSampler2D->samples_per_pixel();

	struct parallel_input {
		vector2i position = vector2i(0);

		bound2i tile;
	};

	struct parallel_output {
		vector2 position = vector2(0);

		spectrum value;
	};

	auto outputs = std::vector<parallel_output>(sample_count);
	auto inputs = std::vector<parallel_input>();

	for (size_t y = bound.min.y; y < bound.max.y; y += tile_size) {
		for (size_t x = bound.min.x; x < bound.max.x; x += tile_size) {
			const auto min_range = vector2i(x, y);
			const auto max_range = vector2i(
				min(static_cast<int>(x + tile_size), bound.max.x),
				min(static_cast<int>(y + tile_size), bound.max.y)
			);

			const auto position = vector2i(
				(x - bound.min.x) / bound_size.x,
				(y - bound.min.y) / bound_size.y);

			inputs.push_back({ position, bound2i(min_range, max_range) });
		}
	}

#ifdef __PARALLEL_RENDER__
	const auto execution_policy = std::execution::par;
#else
	const auto execution_policy = std::execution::seq;
#endif

	const auto samples_per_pixel = mSampler2D->samples_per_pixel();
	
	std::for_each(execution_policy, inputs.begin(), inputs.end(), [&](const parallel_input& input)
		{
			const auto seed = input.position.y * tile_count.x + input.position.x;

			const auto trace_samplers = prepare_samplers(seed);

			for (size_t y = input.tile.min.y; y < input.tile.max.y; y++) {
				for (size_t x = input.tile.min.x; x < input.tile.max.x; x++) {
					trace_samplers.reset();

					for (size_t index = 0; index < samples_per_pixel; index++) {
						const auto position = vector2i(x, y);
						const auto sample = vector2(x, y) + trace_samplers.sampler2d->next();
						const auto sample_index =
							((y - bound.min.y) * bound_size.x + (x - bound.min.x)) * samples_per_pixel + index;

						const auto debug = integrator_debug_info(position);

						outputs[sample_index] = {
							sample,
							trace(scene, debug, trace_samplers, camera->generate_ray(sample), 0)
						};

						trace_samplers.next_sample();
					}
				}
			}
		});

	for (size_t index = 0; index < sample_count; index++)
		film->add_sample(outputs[index].position, outputs[index].value);
}

rainbow::integrators::sampler_group rainbow::integrators::sampler_integrator::prepare_samplers(uint64 seed)
{
	return sampler_group(
		nullptr, mSampler2D->clone(seed)
	);
}

