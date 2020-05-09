#include "integrator.hpp"

#include "../shared/logs/log.hpp"

rainbow::integrators::integrator_debug_info::integrator_debug_info(const vector2i& pixel, size_t sample) :
	pixel(pixel), sample(sample)
{
}

rainbow::integrators::sampler_group::sampler_group(
	const std::shared_ptr<samplers::sampler1d>& sampler1d,
	const std::shared_ptr<samplers::sampler2d>& sampler2d) :
	sampler1d(sampler1d), sampler2d(sampler2d)
{
}

void rainbow::integrators::sampler_group::next_sample() const noexcept
{
	if (sampler1d != nullptr) sampler1d->next_sample();
	if (sampler2d != nullptr) sampler2d->next_sample();
}

void rainbow::integrators::sampler_group::reset() const noexcept
{
	if (sampler1d != nullptr) sampler1d->reset();
	if (sampler2d != nullptr) sampler2d->reset();
}


void rainbow::integrators::integrator::set_debug_trace_pixel(const vector2i& pixel)
{
	mDebugPixels.push_back(pixel);
}

rainbow::spectrum rainbow::integrators::uniform_sample_one_emitter(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers, 
	const surface_interaction& interaction,
	const scattering_function_collection& functions)
{
	// when there are no emitters in scene, we only return 0
	if (scene->emitters().size() == 0) return 0;

	const auto& emitters = scene->emitters();
	
	const auto which = std::min(
		static_cast<size_t>(std::floor(samplers.sampler1d->next().x * emitters.size())),
		emitters.size() - 1);
	const auto pdf = static_cast<real>(1) / emitters.size();

	return estimate_lighting(emitters[which], scene, samplers, interaction, functions) / pdf;
}

rainbow::spectrum rainbow::integrators::estimate_lighting(
	const std::shared_ptr<entity>& emitter,
	const std::shared_ptr<scene>& scene,
	const sampler_group& samplers, 
	const surface_interaction& interaction,
	const scattering_function_collection& functions)
{
	spectrum L = 0;

	const auto emitter_sample = emitter->sample<emitters::emitter>(interaction, samplers.sampler2d->next());

	const auto wi = world_to_local(interaction.shading_space, emitter_sample.wi);
	const auto wo = world_to_local(interaction.shading_space, interaction.wo);
	const auto type = scattering_type::all ^ scattering_type::specular;

	if (!emitter_sample.intensity.is_black() && emitter_sample.pdf > 0) {
		auto functions_value = functions.evaluate(wo, wi, type);
		auto functions_pdf = functions.pdf(wo, wi, type);

		functions_value = functions_value * abs(dot(emitter_sample.wi, interaction.shading_space.z()));

		if (!functions_value.is_black()) {
			const auto shadow_ray = interaction.spawn_ray_to(emitter_sample.position);

			if (!scene->intersect_with_shadow_ray(shadow_ray).has_value()) {

				if (emitter->component<emitters::emitter>()->is_delta())
					L += functions_value * emitter_sample.intensity / emitter_sample.pdf;
				else
					L += functions_value * emitter_sample.intensity * power_heuristic(emitter_sample.pdf, functions_pdf) / emitter_sample.pdf;
			}
		}
	}

	// sample scattering functions with multiple important sampling
	if (!emitter->component<emitters::emitter>()->is_delta()) {
		auto function_sample = functions.sample(interaction, samplers.sampler2d->next(), type);

		function_sample.value = function_sample.value * abs(dot(function_sample.wi, interaction.shading_space.z()));

		if (!function_sample.value.is_black() && function_sample.pdf > 0) {
			const auto emitter_pdf = emitter->pdf<emitters::emitter>(interaction, function_sample.wi);

			if (emitter_pdf == 0) return L;
			
			const auto ray = interaction.spawn_ray(function_sample.wi);

			// if the emitter is environment light, we do not need to do intersect test
			// because it is the sphere bound that contains all entities
			// we only set the surface_interaction::entity
			const auto emitter_interaction = 
				emitter->component<emitters::emitter>()->is_environment() ? 
					std::optional<surface_interaction>(surface_interaction(emitter)) :
					scene->intersect(ray);

			if (!emitter_interaction.has_value() || emitter_interaction->entity != emitter)
				return L;

			const auto intensity = emitter->evaluate<emitters::emitter>(emitter_interaction.value(), -function_sample.wi);
			const auto weight = power_heuristic(function_sample.pdf, emitter_pdf);

			if (!intensity.is_black())
				L += function_sample.value * intensity * weight / function_sample.pdf;
		}
	}

	return L;
}

rainbow::real rainbow::integrators::power_heuristic(real f_pdf, real g_pdf)
{
	return (f_pdf * f_pdf) / (f_pdf * f_pdf + g_pdf * g_pdf);
}
