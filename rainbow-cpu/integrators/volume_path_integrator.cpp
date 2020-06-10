#include "volume_path_integrator.hpp"

#include "../../rainbow-core/logs/log.hpp"

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

	// tracing_info.medium is used to tracing the medium, the default value of medium is empty.
	// when the ray intersect a entity has media, we will update the medium by the normal and ray direction
	tracing_info.medium = medium_info();
	tracing_info.specular = false;
	tracing_info.ray = first_ray;
	tracing_info.value = 0;
	tracing_info.beta = 1;
	tracing_info.eta = 1;

	for (auto bounces = static_cast<int>(depth); bounces < mMaxDepth; bounces++) {
		const auto interaction = scene->intersect(tracing_info.ray);

		// if current medium is not empty, we will sample the medium to decide which interaction we will sample next time
		// if the medium_sample.interaction is std::nullopt, means we will sample the surface_interaction
		// otherwise, we will sample the medium_interaction(in fact, sample the particle in the medium)
		if (tracing_info.medium.has()) {
			// to sample the medium we need provide the beam of ray passed
			const auto medium_sample = tracing_info.medium.sample(samplers.sampler1d, tracing_info.ray);

			// account the beta value
			tracing_info.beta *= medium_sample.value;

			if (tracing_info.beta.is_black()) break;

			if (medium_sample.interaction.has_value()) {
				if (!sample_medium_interaction(scene, samplers, medium_sample.interaction, tracing_info, bounces))
					break;
			}
			else {
				if (!sample_surface_interaction(scene, samplers, interaction, tracing_info, bounces, true))
					break;

				// update the medium property when interaction->entity has media
				// if interaction->normal dot ray.direction > 0, the medium we tracing should be outside of entity
				// otherwise the medium should be inside
				if (interaction->entity->has_component<cpus::media::media>())
					tracing_info.medium = medium_info(interaction->entity, interaction->normal, tracing_info.ray.direction);
			}
		} else {
			if (!sample_surface_interaction(scene, samplers, interaction, tracing_info, bounces, true))
				break;

			if (interaction->entity->has_component<cpus::media::media>())
				tracing_info.medium = medium_info(interaction->entity, interaction->normal, tracing_info.ray.direction);
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
