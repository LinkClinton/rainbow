#include "sampler_integrator.hpp"

#include "../shared/logs/log.hpp"

#ifndef _DEBUG
#define __PARALLEL_RENDER__
#endif

#include <execution>
#include <chrono>
#include <set>

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
		size_t tile_index;

		bound2i tile;
	};

	struct parallel_output {
		film_tile tile;
	};

	auto outputs = std::vector<parallel_output>();
	auto inputs = std::vector<parallel_input>();

	for (size_t y = bound.min.y; y < bound.max.y; y += tile_size) {
		for (size_t x = bound.min.x; x < bound.max.x; x += tile_size) {
			const auto min_range = vector2i(x, y);
			const auto max_range = vector2i(
				min(static_cast<int>(x + tile_size), bound.max.x),
				min(static_cast<int>(y + tile_size), bound.max.y)
			);

			const auto sample_bound = bound2i(min_range, max_range);
			
			inputs.push_back({ inputs.size(), sample_bound });
			outputs.push_back({ film_tile(sample_bound, film) });
		}
	}

#ifdef _DEBUG
	std::set<std::pair<int, int>> debug_pixel_lists;

	for (const auto& debug_pixel : mDebugPixels) 
		debug_pixel_lists.insert({ debug_pixel.x, debug_pixel.y });
#endif

#ifdef __PARALLEL_RENDER__
	const auto execution_policy = std::execution::par;
#else
	const auto execution_policy = std::execution::seq;
#endif

	const auto samples_per_pixel = mSampler2D->samples_per_pixel();

	logs::info("start rendering...");
	logs::info("image min range : x = {0}, y = {1}.", bound.min.x, bound.min.y);
	logs::info("image max range : x = {0}, y = {1}.", bound.max.x, bound.max.y);
	logs::info("tile size : width = {0}, height = {1}.", tile_size, tile_size);

	std::atomic_int finished_tile_count = 0;

	const auto start_rendering_time = std::chrono::high_resolution_clock::now();
	
	std::for_each(execution_policy, inputs.begin(), inputs.end(), [&](const parallel_input& input)
		{
			const auto seed = input.tile_index;

			const auto trace_samplers = prepare_samplers(seed);

			for (auto y = input.tile.min.y; y < input.tile.max.y; y++) {
				for (auto x = input.tile.min.x; x < input.tile.max.x; x++) {
					trace_samplers.reset();

					for (size_t index = 0; index < samples_per_pixel; index++) {
						const auto position = vector2i(x, y);
						const auto sample = vector2(x, y) + trace_samplers.sampler2d->next();
						
						const auto debug = integrator_debug_info(position, index);
						
#ifdef _DEBUG
						// when mDebugPixels is not empty, the debug_pixel_lists is not empty too.
						// when debug_pixel_lists is not empty, we will only trace the pixel that in the debug lists
						// in other words, the pixels we called integrator::set_debug_trace_pixel().
						if (!debug_pixel_lists.empty() && debug_pixel_lists.find({ x, y }) == debug_pixel_lists.end()) {

							// we do not trace these sample, but the filter weight can not be zero
							// so we will set the sample value to zero.
							outputs[input.tile_index].tile.add_sample(sample, 0);
							
							trace_samplers.next_sample();

							continue;
						}
#endif					

						outputs[input.tile_index].tile.add_sample(
							sample,
							trace(scene, debug, trace_samplers, camera->generate_ray(sample), 0)
						);

						trace_samplers.next_sample();
					}
				}
			}

			logs::info("finish tile {0}, finished {1} / total : {2}", input.tile_index, ++finished_tile_count, inputs.size());
		});

	for (size_t index = 0; index < outputs.size(); index++)
		film->add_tile(outputs[index].tile);

	const auto end_rendering_time = std::chrono::high_resolution_clock::now();
	
	logs::info("finish rendering..., time used {0}s.", 
		std::chrono::duration_cast<std::chrono::duration<double>>(end_rendering_time - start_rendering_time).count());
}

rainbow::integrators::sampler_group rainbow::integrators::sampler_integrator::prepare_samplers(uint64 seed)
{
	return sampler_group(
		nullptr, mSampler2D->clone(seed)
	);
}