#include "integrator.hpp"

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



rainbow::spectrum rainbow::integrators::uniform_sample_one_light(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers, 
	const surface_interaction& interaction,
	const scattering_function_collection& functions)
{
	// when there are no lights in scene, we only return 0
	if (scene->lights().size() == 0) return 0;

	const auto& lights = scene->lights();
	
	const auto which = std::min(
		static_cast<size_t>(std::floor(samplers.sampler1d->next_sample().x * lights.size())),
		lights.size() - 1);
	const auto pdf = static_cast<real>(1) / lights.size();

	return estimate_lighting(scene, lights[which], samplers, interaction, functions) / pdf;
}

rainbow::spectrum rainbow::integrators::estimate_lighting(
	const std::shared_ptr<scene>& scene,
	const std::shared_ptr<light>& light, 
	const sampler_group& samplers, 
	const surface_interaction& interaction,
	const scattering_function_collection& functions)
{
	spectrum L = 0;

	const auto light_sample = light->sample(interaction.point, samplers.sampler2d->next_sample());

	if (light_sample.irradiance.is_black() || light_sample.pdf == 0) return L;

	const auto wi = world_to_local(interaction.shading_space, light_sample.wi);
	const auto wo = world_to_local(interaction.shading_space, interaction.wo);
	const auto type = scattering_type::all ^ scattering_type::specular;
	
	const auto functions_value = functions.evaluate(wo, wi, type);
	
	if (!functions_value.is_black()) {
		const auto shadow_ray = interaction.spawn_ray_to(light_sample.position);

		if (scene->intersect_with_shadow_ray(shadow_ray).has_value()) return L;
		
		L += functions_value * light_sample.irradiance * abs(dot(light_sample.wi, interaction.normal)) / light_sample.pdf;
	}
	
	return L;
}
