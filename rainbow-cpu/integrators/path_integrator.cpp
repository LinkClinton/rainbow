#include "path_integrator.hpp"

#include "../../rainbow-core/logs/log.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::integrators::path_integrator::path_integrator(
	const std::shared_ptr<sampler2d>& sampler2d, 
	const std::shared_ptr<sampler1d>& sampler1d, 
	size_t max_depth, real threshold) :
	sampler_integrator(sampler2d, max_depth), mSampler1D(sampler1d), mThreshold(threshold)
{
}

spectrum rainbow::cpus::integrators::path_integrator::trace(
	const std::shared_ptr<scene>& scene,
	const integrator_debug_info& debug, 
	const sampler_group& samplers, 
	const ray& first_ray, size_t depth)
{
	path_tracing_info tracing_info;

	tracing_info.specular = false;
	tracing_info.ray = first_ray;
	tracing_info.value = 0;
	tracing_info.beta = 1;
	tracing_info.eta = 1;
	
	for (auto bounces = static_cast<int>(depth); bounces < mMaxDepth; bounces++) {
		const auto interaction = scene->intersect(tracing_info.ray);

		if (!sample_surface_interaction(scene, samplers, interaction, tracing_info, bounces, false))
			break;

		const auto max_component = (tracing_info.beta * tracing_info.eta).max_component();
		
		if (max_component < mThreshold && bounces > 3) {
			const auto q = max(static_cast<real>(0.05), 1 - max_component);
			
			if (samplers.sampler1d->next().x < q) break;

			tracing_info.beta = tracing_info.beta / (1 - q);
		}
	}

	return tracing_info.value;
}

rainbow::cpus::integrators::sampler_group rainbow::cpus::integrators::path_integrator::prepare_samplers(uint64 seed)
{
	const auto generator = std::make_shared<random_generator>(seed);
	
	return sampler_group(
		mSampler1D->clone(generator),
		mSampler2D->clone(generator)
	);
}
