#include "whitted_integrator.hpp"

rainbow::integrators::whitted_integrator::whitted_integrator( 
	const std::shared_ptr<sampler2d>& sampler2d, size_t max_depth) :
	sampler_integrator(sampler2d, max_depth)
{
}

rainbow::spectrum rainbow::integrators::whitted_integrator::trace(
	const std::shared_ptr<scene>& scene,
	const integrator_debug_info& debug,
	const sampler_group& samplers, 
	const ray& ray, size_t depth)
{
	spectrum L = 0;

	auto interaction = scene->intersect(ray);

	// if we do not find the shape that the ray intersect
	// we will return L
	if (!interaction.has_value()) return L;

	// get the scattering functions from material which the ray intersect
	const auto scattering_functions = 
		interaction->entity->component<material>()->build_scattering_functions(interaction.value());

	// when the scattering functions is empty, we can think it is a invisible shape
	// we will continue spawn a ray without changing the direction
	if (scattering_functions.count() == 0)
		return trace(scene, debug, samplers, interaction->spawn_ray(ray.direction), depth);

	const auto wo = world_to_local(interaction->shading_space, interaction->wo);

	for (const auto& emitter : scene->emitters()) {
		const auto emitter_sample = emitter->sample<emitters::emitter>(interaction.value(), samplers.sampler2d->next());
		
		if (emitter_sample.intensity.is_black() || emitter_sample.pdf == 0) continue;
		
		// notice : the value of specular functions is zero
		const auto wi = world_to_local(interaction->shading_space, emitter_sample.wi);
		const auto function_value = scattering_functions.evaluate(wo, wi);
		const auto shadow_ray = interaction->spawn_ray_to(emitter_sample.position);
	
		// spawn a shadow ray to test the emitter is visible
		if (scene->intersect_with_shadow_ray(shadow_ray).has_value())
			continue;

		if (!function_value.is_black()) 
			L += function_value * emitter_sample.intensity * abs(dot(emitter_sample.wi, interaction->normal)) / emitter_sample.pdf;
	}

	// trace the rays with specular
	if (depth + 1 < mMaxDepth) {
		L += specular_reflect(scene, debug, samplers, ray, interaction.value(), scattering_functions, depth);
		L += specular_refract(scene, debug, samplers, ray, interaction.value(), scattering_functions, depth);
	}

	return L;
}

rainbow::spectrum rainbow::integrators::whitted_integrator::specular_reflect(
	const std::shared_ptr<scene>& scene,
	const integrator_debug_info& debug,
	const sampler_group& samplers, 
	const ray& ray, const surface_interaction& interaction,
	const scattering_function_collection& functions, size_t depth)
{	
	const auto scattering_sample = functions.sample(
		interaction, samplers.sampler2d->next(),
		scattering_type::reflection | scattering_type::specular);

	const auto dot_value = abs(dot(scattering_sample.wi, interaction.normal));
	
	// we do not trace a ray that does not has contribution
	if (scattering_sample.pdf <= 0 || scattering_sample.value.is_black() || dot_value == 0)
		return 0;
	
	const auto L = trace(scene, debug, samplers, interaction.spawn_ray(scattering_sample.wi), depth + 1);
	
	return scattering_sample.value * L * dot_value / scattering_sample.pdf;
}

rainbow::spectrum rainbow::integrators::whitted_integrator::specular_refract(
	const std::shared_ptr<scene>& scene,
	const integrator_debug_info& debug,
	const sampler_group& samplers, 
	const ray& ray, const surface_interaction& interaction,
	const scattering_function_collection& functions, size_t depth)
{
	const auto scattering_sample = functions.sample(
		interaction, samplers.sampler2d->next(),
		scattering_type::transmission | scattering_type::specular);

	const auto dot_value = abs(dot(scattering_sample.wi, interaction.normal));

	// we do not trace a ray that does not has contribution
	if (scattering_sample.pdf <= 0 || scattering_sample.value.is_black() || dot_value == 0)
		return 0;

	const auto L = trace(scene, debug, samplers, interaction.spawn_ray(scattering_sample.wi), depth + 1);

	return scattering_sample.value * L * dot_value / scattering_sample.pdf;
}
