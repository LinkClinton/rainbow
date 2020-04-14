#include "whitted_integrator.hpp"

rainbow::integrators::whitted_integrator::whitted_integrator(
	const std::shared_ptr<sampler2d>& camera_sampler, 
	const std::shared_ptr<sampler2d>& sampler, size_t max_depth) :
	sampler_integrator(camera_sampler, max_depth), mSampler(sampler)
{
}

rainbow::spectrum rainbow::integrators::whitted_integrator::trace(
	const std::shared_ptr<scene>& scene,
	const sampler_group& samplers, const ray& ray, size_t depth)
{
	spectrum L = 0;

	auto interaction = scene->intersect(ray);

	// if we do not find the shape that the ray intersect
	// we will return L
	if (!interaction.has_value()) return L;

	// get the scattering functions from material which the ray intersect
	const auto scattering_functions = 
		interaction->shape->material()->build_scattering_functions(interaction.value());

	// when the scattering functions is empty, we can think it is a invisible shape
	// we will continue spawn a ray without changing the direction
	if (scattering_functions.size() == 0)
		return trace(scene, samplers, interaction->spawn_ray(ray.direction), depth);

	const auto wo = world_to_local(interaction->shading_space, interaction->wo);

	for (const auto& light : scene->lights()) {
		const auto light_sample = light->sample(interaction->point, samplers.sampler2d->next_sample());
		
		if (light_sample.irradiance.is_black() || light_sample.pdf == 0) continue;

		// notice : the value of specular functions is zero
		const auto wi = world_to_local(interaction->shading_space, light_sample.wi);
		const auto function_value = scattering_functions.evaluate(wo, wi);
		const auto shadow_ray = interaction->spawn_ray(light_sample.wi);

		// spawn a shadow ray to test the light is visible
		if (scene->intersect_with_shadow_ray(shadow_ray).has_value())
			continue;

		if (!function_value.is_black()) 
			L += function_value * light_sample.irradiance * abs(dot(light_sample.wi, interaction->normal)) / light_sample.pdf;
	}

	// trace the rays with specular
	if (depth + 1 < mMaxDepth) {
		L += specular_reflect(scene, samplers, ray, interaction.value(), scattering_functions, depth);
		L += specular_refract(scene, samplers, ray, interaction.value(), scattering_functions, depth);
	}

	return L;
}

rainbow::integrators::sampler_group rainbow::integrators::whitted_integrator::prepare_samplers()
{
	return sampler_group(nullptr, mSampler->clone());
}

rainbow::spectrum rainbow::integrators::whitted_integrator::specular_reflect(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers, 
	const ray& ray, const surface_interaction& interaction,
	const scattering_function_collection& functions, size_t depth)
{	
	const auto scattering_sample = functions.sample(
		interaction, samplers.sampler2d->next_sample(),
		scattering_type::reflection | scattering_type::specular);

	const auto wi = local_to_world(interaction.shading_space, scattering_sample.wi);
	const auto dot_value = abs(dot(wi, interaction.normal));
	
	// we do not trace a ray that does not has contribution
	if (scattering_sample.pdf <= 0 || scattering_sample.value.is_black() || dot_value == 0)
		return 0;
	
	const auto L = trace(scene, samplers, interaction.spawn_ray(wi), depth + 1);
	
	return scattering_sample.value * L * dot_value / scattering_sample.pdf;
}

rainbow::spectrum rainbow::integrators::whitted_integrator::specular_refract(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers, 
	const ray& ray, const surface_interaction& interaction,
	const scattering_function_collection& functions, size_t depth)
{
	const auto scattering_sample = functions.sample(
		interaction, samplers.sampler2d->next_sample(),
		scattering_type::transmission | scattering_type::specular);

	const auto wi = local_to_world(interaction.shading_space, scattering_sample.wi);
	const auto dot_value = abs(dot(wi, interaction.normal));

	// we do not trace a ray that does not has contribution
	if (scattering_sample.pdf <= 0 || scattering_sample.value.is_black() || dot_value == 0)
		return 0;

	const auto L = trace(scene, samplers, interaction.spawn_ray(wi), depth + 1);

	return scattering_sample.value * L * dot_value / scattering_sample.pdf;
}
