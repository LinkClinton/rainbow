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

std::tuple<std::optional<rainbow::surface_interaction>, rainbow::real> rainbow::integrators::find_emitter(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers,
	const surface_interaction& interaction, const vector3& wi)
{
	// give the direction of incident ray(wi), we need find the emitter it from
	// first, we spawn ray from surface interaction
	const auto emitter_ray = interaction.spawn_ray(wi);
	const auto emitter_interaction = scene->intersect(emitter_ray);

	const auto intersect_emitter = (emitter_interaction.has_value() && emitter_interaction->entity->has_component<emitter>());
	const auto is_environment = (!scene->environments().empty() && !emitter_interaction.has_value());

	// if the ray do not intersect the emitter entity and environment emitter
	// we return nullptr and 0
	if (!intersect_emitter && !is_environment) return { std::nullopt, static_cast<real>(0) };

	const auto pdf = static_cast<real>(1) / scene->emitters().size();

	// if the ray intersect a entity with emitter we will return the entity
	// if not, we will find the environment emitter
	if (intersect_emitter) return { emitter_interaction, pdf };

	const auto which = std::min(
		static_cast<size_t>(std::floor(samplers.sampler1d->next().x * scene->environments().size())),
		scene->environments().size() - 1);

	return { surface_interaction(scene->environments()[which]), pdf };
}

std::tuple<std::shared_ptr<const rainbow::entity>, rainbow::real> rainbow::integrators::uniform_sample_one_emitter(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers)
{
	// sample a emitter from scene
	// when there are no emitters in scene, we only return 0 
	if (scene->emitters().empty()) return { nullptr, static_cast<real>(0) };

	const auto& emitters = scene->emitters();

	const auto which = std::min(
		static_cast<size_t>(std::floor(samplers.sampler1d->next().x * emitters.size())),
		emitters.size() - 1);
	const auto pdf = static_cast<real>(1) / emitters.size();

	return { emitters[which], pdf };
}

rainbow::spectrum rainbow::integrators::uniform_sample_one_emitter(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers, 
	const surface_interaction& interaction,
	const scattering_function_collection& functions)
{
	// this function do multiple important sampling
	// first, we sample the emitter. second, we sample the bsdf.
	// notice : we ignore the specular 

	spectrum L = 0;
	
	const auto type = scattering_type::all ^ scattering_type::specular;
	
	// emitter sampling, we sample the emitters with multiple important sampling
	{
		// sample which emitter we will sample 
		auto [emitter, pdf] = uniform_sample_one_emitter(scene, samplers);

		// sample where the light spawn
		auto emitter_sample = emitter->sample<emitters::emitter>(interaction, samplers.sampler2d->next());

		// the real pdf of emitter_sample should multi the pdf of sampling which emitter
		emitter_sample.pdf = emitter_sample.pdf * pdf;

		if (!emitter_sample.intensity.is_black() && emitter_sample.pdf > 0) {
			const auto wi = interaction.from_world_to_space(emitter_sample.wi);
			const auto wo = interaction.from_world_to_space(interaction.wo);

			auto function_value = functions.evaluate(wo, wi, type);
			auto function_pdf = functions.pdf(wo, wi, type);

			function_value = function_value * abs(dot(emitter_sample.wi, interaction.shading_space.z()));

			if (!function_value.is_black() && function_pdf > 0) {
				
				const auto shadow_ray = interaction.spawn_ray_to(emitter_sample.position);
				const auto shadow_interaction = scene->intersect_with_shadow_ray(shadow_ray);

				// if the shadow ray intersect a entity that is not the emitter
				// we need skip this shading, because the ray from emitter to entity is occluded
				if (!shadow_interaction.has_value() || shadow_interaction->entity == emitter) {

					// if the emitter is delta, the weight should be 1
					// f(i) * g(i) * w(i) / (p(i) * nf) + f(j) * g(j) * w(j) / (p(j) * ng)
					// f is the scattering functions, g is the emitter, p is the pdf
					// nf and ng is the number of samples
					// weight = (nf * f)^2 / (ng * g)^2 = (nf * f / all)^2 / (ng * g / all)^2
					// all = nf + ng
					const auto weight = emitter->component<emitters::emitter>()->is_delta() ? 1 :
						power_heuristic(emitter_sample.pdf, function_pdf);

					L += function_value * emitter_sample.intensity * weight / emitter_sample.pdf;
				}
			}
		}
	}

	// bsdf sampling, we sample the bsdfs with multiple important sampling
	{
		auto function_sample = functions.sample(interaction, samplers.sampler2d->next(), type);

		function_sample.value = function_sample.value * abs(dot(function_sample.wi, interaction.shading_space.z()));

		if (!function_sample.value.is_black() && function_sample.pdf > 0) {
			// find the emitter the sample ray intersect
			auto [emitter_interaction, pdf] = find_emitter(scene, samplers, interaction, function_sample.wi);

			if (emitter_interaction.has_value() && pdf > 0) {

				const auto emitter = emitter_interaction->entity;
				const auto emitter_value = emitter->evaluate<emitters::emitter>(emitter_interaction.value(), -function_sample.wi);
				const auto emitter_pdf = emitter->pdf<emitters::emitter>(interaction, function_sample.wi) * pdf;

				if (!emitter_value.is_black() && emitter_pdf > 0) {

					// if the bsdf is delta, the weight should be 1
					// f(i) * g(i) * w(i) / (p(i) * nf) + f(j) * g(j) * w(j) / (p(j) * ng)
					// f is the scattering functions, g is the emitter, p is the pdf
					// nf and ng is the number of samples
					// weight = (nf * f)^2 / (ng * g)^2 = (nf * f / all)^2 / (ng * g / all)^2
					// all = nf + ng
					const auto weight = has(function_sample.type, scattering_type::specular) ? 1 :
						power_heuristic(function_sample.pdf, emitter_pdf);

					L += function_sample.value * emitter_value * weight / function_sample.pdf;
				}
			}
		}
	}

	return L;
}

rainbow::real rainbow::integrators::power_heuristic(real f_pdf, real g_pdf)
{
	return (f_pdf * f_pdf) / (f_pdf * f_pdf + g_pdf * g_pdf);
}
