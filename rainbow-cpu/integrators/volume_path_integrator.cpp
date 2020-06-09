#include "volume_path_integrator.hpp"

rainbow::cpus::integrators::volume_path_integrator::volume_path_integrator(
	const std::shared_ptr<sampler2d>& sampler2d,
	const std::shared_ptr<sampler1d>& sampler1d, 
	size_t max_depth, real threshold) :
	sampler_integrator(sampler2d, max_depth), mSampler1D(sampler1d), mThreshold(threshold)
{
}

rainbow::cpus::shared::spectrum rainbow::cpus::integrators::volume_path_integrator::trace(
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

	for (auto bounces = depth; bounces < mMaxDepth; bounces++) {
		const auto interaction = scene->intersect(tracing_info.ray);

		// if the ray does not intersect anything or the entity the ray intersect does not have media
		// we need sample the surface interaction
		if (!interaction.has_value() || !interaction->entity->has_component<cpus::media::media>()) {
			if (!sample_surface_interaction(scene, samplers, interaction, tracing_info, bounces, true))
				break;
		}
		else {
			// sample the medium, if the medium_sample.interaction is not std::nullopt
			// we will sample the medium_interaction, otherwise we will sample the surface interaction
			const auto medium_sample = interaction->entity->sample<cpus::media::media>(samplers.sampler1d,
				interaction.value(), tracing_info.ray.reverse());

			// account the beam value to beta
			tracing_info.beta *= medium_sample.value;

			if (tracing_info.beta.is_black()) break;

			// sample the medium if the medium_sample.interaction is not std::nullopt
			if (medium_sample.interaction.has_value()) {
				if (!sample_medium_interaction(scene, samplers, medium_sample.interaction, tracing_info, bounces))
					break;
			}
			else {
				if (!sample_surface_interaction(scene, samplers, interaction, tracing_info, bounces, true))
					break;
			}
		}
		
		const auto max_component = (tracing_info.beta * tracing_info.eta).max_component();

		if (max_component < mThreshold && bounces > 3) {
			const auto q = max(static_cast<real>(0.05), 1 - max_component);

			if (samplers.sampler1d->next().x < q) break;

			tracing_info.beta = tracing_info.beta / (1 - q);
		}
	}

	return tracing_info.value;
}

rainbow::cpus::integrators::sampler_group rainbow::cpus::integrators::volume_path_integrator::prepare_samplers(uint64 seed)
{
	const auto generator = std::make_shared<random_generator>(seed);

	return sampler_group(
		mSampler1D->clone(generator),
		mSampler2D->clone(generator)
	);
}
