#include "path_integrator.hpp"

#include "../scatterings/scattering_surface_function.hpp"
#include "../shared/logs/log.hpp"

rainbow::integrators::path_integrator::path_integrator(
	const std::shared_ptr<sampler2d>& sampler2d, 
	const std::shared_ptr<sampler1d>& sampler1d, 
	size_t max_depth, real threshold) :
	sampler_integrator(sampler2d, max_depth), mSampler1D(sampler1d), mThreshold(threshold)
{
}

rainbow::spectrum rainbow::integrators::path_integrator::trace(
	const std::shared_ptr<scene>& scene,
	const integrator_debug_info& debug, 
	const sampler_group& samplers, 
	const ray& first_ray, size_t depth)
{
	spectrum beta = 1;
	spectrum L = 0;

	real eta = 1;

	auto specular_bounce = false;
	
	// ray is the current ray in current bounces
	auto ray = first_ray;

	for (auto bounces = depth; bounces < mMaxDepth; bounces++) {
		const auto interaction = scene->intersect(ray);

		// if we do not find the shape that the ray intersect we can end this tracing
		if (!interaction.has_value()) {
			// if it is not first bounce or it is not specular bounce at last bounce
			// we just end this tracing
			// if it is first bounce or it is specular bounce, we will evaluate the environment light
			if (bounces != 0 && !specular_bounce) break;

			for (const auto& environment : scene->environments())
				L += beta * environment->evaluate<emitter>(interactions::interaction(), -ray.direction);

			break;
		}

		// when the first ray intersect a light, we will evaluate the intensity of it
		// when the specular ray intersect an emitter, we will evaluate the intensity of it
		// because the specular ray we do not solve it at "uniform_sample_one_emitter".
		if ((bounces == 0 || specular_bounce) && interaction->entity->has_component<emitter>())
			L += beta * interaction->entity->evaluate<emitter>(interaction.value(), -ray.direction);

		// when the material is nullptr, we can think it is a invisible entity
		// we will continue spawn a ray without changing the direction
		if (!interaction->entity->has_component<material>()) {
			// compute the new ray 
			ray = interaction->spawn_ray(ray.direction);

			// because we intersect a invisible shape, we do not need add the bounces
			bounces--;

			continue;
		}
		
		// get the surface properties from material which the ray intersect
		// if the entity does not have material, we return default surface properties(0 functions)
		const auto surface_properties =
			interaction->entity->component<material>()->build_surface_properties(interaction.value());

		// get the scattering functions from surface properties
		const auto& scattering_functions = surface_properties.functions;

		// we will sample the emitters to compute the path contribution
		// when the functions do not have any functions without specular we do not sample it
		// because the f(wo, wi) of specular functions is 0, the result must be 0.
		if (scattering_functions.count(scattering_type::all ^ scattering_type::specular) != 0)
			L += beta * uniform_sample_one_emitter(scene, samplers, interaction.value(), scattering_functions);

		const auto scattering_sample = scattering_functions.sample(interaction.value(), samplers.sampler2d->next());

		if (scattering_sample.value.is_black() || scattering_sample.pdf == 0) break;

		beta = beta * scattering_sample.value * abs(dot(scattering_sample.wi, interaction->shading_space.z())) / scattering_sample.pdf;

		// indicate the path it is specular or not.
		// if it is, we will evaluate it when we intersect an emitter at next bounce
		specular_bounce = has(scattering_sample.type, scattering_type::specular);
		
		if (has(scattering_sample.type, scattering_type::specular | scattering_type::transmission)) {
			const auto surface_eta = scattering_functions.eta();

			eta = eta * ((dot(interaction->wo, interaction->normal) > 0) ? 
				(surface_eta * surface_eta) : (1 / (surface_eta * surface_eta)));
		}
		
		ray = interaction->spawn_ray(scattering_sample.wi);

		if (surface_properties.bssrdf != nullptr && has(scattering_sample.type, scattering_type::transmission)) {

			const auto sample = surface_properties.bssrdf->sample(scene, vector3(samplers.sampler1d->next(), samplers.sampler2d->next()));

			if (sample.value.is_black() || sample.pdf == 0) break;

			beta = beta * sample.value / sample.pdf;

			L += beta * uniform_sample_one_emitter(scene, samplers, sample.interaction, sample.functions);

			const auto function_sample = sample.functions.sample(sample.interaction, samplers.sampler2d->next());

			if (function_sample.value.is_black() || function_sample.pdf == 0) break;
			
			beta = beta * function_sample.value * abs(dot(function_sample.wi, sample.interaction.shading_space.z())) / function_sample.pdf;

			specular_bounce = has(function_sample.type, scattering_type::specular);

			ray = sample.interaction.spawn_ray(function_sample.wi);
		}
		
		const auto max_component = (beta * eta).max_component();
		
		if (max_component < mThreshold && bounces > 3) {
			const auto q = max(static_cast<real>(0.05), 1 - max_component);
			
			if (samplers.sampler1d->next().x < q) break;

			beta = beta / (1 - q);
		}
	}

	return L;
}

rainbow::integrators::sampler_group rainbow::integrators::path_integrator::prepare_samplers(uint64 seed)
{
	const auto generator = std::make_shared<random_generator>(seed);
	
	return sampler_group(
		mSampler1D->clone(generator),
		mSampler2D->clone(generator)
	);
}
