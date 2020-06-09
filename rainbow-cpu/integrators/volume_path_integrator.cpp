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

	tracing_info.specular = false;
	tracing_info.ray = first_ray;
	tracing_info.value = 0;
	tracing_info.beta = 1;
	tracing_info.eta = 1;

	auto bounces = static_cast<int>(depth);
	
	auto medium_interaction = scene->intersect(tracing_info.ray);

	// for the first interaction, the origin medium is unknown, so we need the medium of interaction to sample
	// if the first interaction has not media or no first interaction, we just sample the surface_interaction
	// (it will solve the no first interaction situation)
	if (!medium_interaction.has_value() || !medium_interaction->entity->has_component<cpus::media::media>()) {
		
		if (!sample_surface_interaction(scene, samplers, medium_interaction, tracing_info, bounces, true))
			return tracing_info.value;
	} else {
		// now, sample the medium from camera to first interaction
		// because the medium_interaction is end point of beam, we do not need reverse the wo of medium_interaction
		// the dot(medium_interaction.normal, medium_interaction.wo) decide which medium we will use(outside or inside)
		const auto medium_sample = medium_interaction->entity->sample<cpus::media::media>(samplers.sampler1d,
			medium_interaction.value(), tracing_info.ray);

		// account the beta value
		tracing_info.beta *= medium_sample.value;

		if (tracing_info.beta.is_black()) return tracing_info.value;

		// if the medium_sample.interaction != std::nullopt, means we will sample the medium
		// otherwise, we will sample the surface interaction
		if (medium_sample.interaction.has_value()) {
			if (!sample_medium_interaction(scene, samplers, medium_sample.interaction, tracing_info, bounces))
				return tracing_info.value;
		}
		else {
			if (!sample_surface_interaction(scene, samplers, medium_interaction, tracing_info, bounces, true))
				return tracing_info.value;
		}
	}

	// reverse the direction of wo, because we will use it as start point of beam not end point of beam in next time
	medium_interaction->wo = -medium_interaction->wo;

	// now start the path tracing, the bounces should be the bounces + 1
	for (bounces = bounces + 1; bounces < mMaxDepth; bounces++) {
		const auto interaction = scene->intersect(tracing_info.ray);

		// the medium_interaction keep the medium ray in
		// if there is no media in medium_interaction->entity, we just sample the surface interaction
		// otherwise, we will sample the medium to find which type interaction wi need sample
		if (medium_interaction->entity->has_component<cpus::media::media>()) {
			// sample the medium, the medium_interaction.wo had reverse
			// because the interaction is the start point of beam, not the end point of beam
			const auto medium_sample = medium_interaction->entity->sample<cpus::media::media>(samplers.sampler1d,
				medium_interaction.value(), tracing_info.ray);

			tracing_info.beta *= medium_sample.value;

			if (tracing_info.beta.is_black()) break;

			if (medium_sample.interaction.has_value()) {
				if (!sample_medium_interaction(scene, samplers, medium_sample.interaction, tracing_info, bounces))
					break;
			}
			else {
				if (!sample_surface_interaction(scene, samplers, interaction, tracing_info, bounces, true))
					break;

				// if we had sampled the surface interaction, the medium information had changed.
				// so we should update the medium_interaction and do not forget to reverse the wo of medium_interaction
				medium_interaction = interaction;
				medium_interaction->wo = -medium_interaction->wo;
			}
		}
		else {
			if (!sample_surface_interaction(scene, samplers, interaction, tracing_info, bounces, true))
				break;

			// if we had sampled the surface interaction, the medium information had changed.
			// so we should update the medium_interaction and do not forget to reverse the wo of medium_interaction
			medium_interaction = interaction;
			medium_interaction->wo = -medium_interaction->wo;
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
