#include "integrator.hpp"

#include "../../rainbow-core/logs/log.hpp"

#include "../scatterings/scattering_surface_function.hpp"
#include "../emitters/environment_light.hpp"

using namespace rainbow::cpus::shared::interactions;
using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::integrators::integrator_debug_info::integrator_debug_info(const vector2i& pixel, size_t sample) :
	pixel(pixel), sample(sample)
{
}

rainbow::cpus::integrators::sampler_group::sampler_group(
	const std::shared_ptr<samplers::sampler1d>& sampler1d,
	const std::shared_ptr<samplers::sampler2d>& sampler2d) :
	sampler1d(sampler1d), sampler2d(sampler2d)
{
}

void rainbow::cpus::integrators::sampler_group::next_sample() const noexcept
{
	if (sampler1d != nullptr) sampler1d->next_sample();
	if (sampler2d != nullptr) sampler2d->next_sample();
}

void rainbow::cpus::integrators::sampler_group::reset() const noexcept
{
	if (sampler1d != nullptr) sampler1d->reset();
	if (sampler2d != nullptr) sampler2d->reset();
}


rainbow::cpus::integrators::path_tracing_info::path_tracing_info(
	const spectrum& value, const spectrum& beta,
	const medium_info& medium, const shared::ray& ray, 
	real eta, bool specular) :
	value(value), beta(beta), specular(specular), eta(eta), medium(medium), ray(ray)
{
}

void rainbow::cpus::integrators::integrator::set_debug_trace_pixel(const vector2i& pixel)
{
	mDebugPixels.push_back(pixel);
}

std::tuple<std::optional<surface_interaction>, rainbow::core::real> rainbow::cpus::integrators::find_emitter(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers,
	const interaction& interaction, const vector3& wi)
{
	// give the direction of incident ray(wi), we need find the emitter it from
	// first, we spawn ray from surface interaction
	// because the area light should have material, we can use intersect_with_shadow_ray to ignore the medium shape
	const auto emitter_ray = interaction.spawn_ray(wi);
	const auto emitter_interaction = scene->intersect_with_shadow_ray(emitter_ray);

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
	const auto emitter = 
		std::static_pointer_cast<environment_light>(scene->environments()[which]->component<emitters::emitter>());

	// compute the point of ray intersect with environment light
	const auto point = interaction.point + static_cast<real>(2) * wi * emitter->radius();
	
	return { surface_interaction(scene->environments()[which], point), pdf };
}

std::tuple<std::shared_ptr<const entity>, rainbow::core::real> rainbow::cpus::integrators::uniform_sample_one_emitter(
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

spectrum rainbow::cpus::integrators::uniform_sample_one_emitter(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers, 
	const path_tracing_info& tracing_info, const surface_interaction& interaction, 
	const scattering_function_collection& functions,
	bool media)
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

			function_value = function_value * math::abs(dot(emitter_sample.wi, interaction.shading_space.z()));

			if (!function_value.is_black() && function_pdf > 0) {
				
				const auto shadow_ray = interaction.spawn_ray_to(emitter_sample.interaction.point);
				const auto shadow_interaction = scene->intersect_with_shadow_ray(shadow_ray);

				// if the shadow ray intersect a entity that is not the emitter
				// we need skip this shading, because the ray from emitter to entity is occluded
				if (!shadow_interaction.has_value() || shadow_interaction->entity == emitter) {

					// if we need handle the media, we will evaluate the media beam from surface to light
					if (media) function_value *= scene->evaluate_media_beam(samplers.sampler1d,
						{ tracing_info.medium, interaction }, emitter_sample.interaction);
					
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

		function_sample.value = function_sample.value * math::abs(dot(function_sample.wi, interaction.shading_space.z()));

		if (!function_sample.value.is_black() && function_sample.pdf > 0) {
			// find the emitter the sample ray intersect
			auto [emitter_interaction, pdf] = find_emitter(scene, samplers, interaction, function_sample.wi);

			if (emitter_interaction.has_value() && pdf > 0) {

				const auto emitter = emitter_interaction->entity;
				const auto emitter_value = emitter->evaluate<emitters::emitter>(emitter_interaction.value(), -function_sample.wi);
				const auto emitter_pdf = emitter->pdf<emitters::emitter>(interaction, function_sample.wi) * pdf;

				if (!emitter_value.is_black() && emitter_pdf > 0) {

					// if we need handle the media, we will evaluate the media beam from surface to light
					if (media) function_sample.value *= scene->evaluate_media_beam(samplers.sampler1d,
						{ tracing_info.medium, interaction }, emitter_interaction.value());
					
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

spectrum rainbow::cpus::integrators::uniform_sample_one_emitter(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers,
	const path_tracing_info& tracing_info, const medium_interaction& interaction)
{
	spectrum L = 0;

	{
		// sample which emitter we will sample 
		auto [emitter, pdf] = uniform_sample_one_emitter(scene, samplers);

		// sample where the light spawn
		auto emitter_sample = emitter->sample<emitters::emitter>(interaction, samplers.sampler2d->next());

		// the real pdf of emitter_sample should multi the pdf of sampling which emitter
		emitter_sample.pdf = emitter_sample.pdf * pdf;

		if (!emitter_sample.intensity.is_black() && emitter_sample.pdf > 0) {
			const auto phase_value = interaction.function->evaluate(interaction.wo, emitter_sample.wi);
			const auto function_value = spectrum(phase_value);
			const auto function_pdf = phase_value;

			if (!function_value.is_black() && function_pdf > 0) {

				const auto shadow_ray = interaction.spawn_ray_to(emitter_sample.interaction.point);
				const auto shadow_interaction = scene->intersect_with_shadow_ray(shadow_ray);

				// if the shadow ray intersect a entity that is not the emitter
				// we need skip this shading, because the ray from emitter to entity is occluded
				if (!shadow_interaction.has_value() || shadow_interaction->entity == emitter) {

					// if we need handle the media, we will evaluate the media beam from surface to light
					const auto beam = scene->evaluate_media_beam(samplers.sampler1d,
						{ tracing_info.medium, interaction }, emitter_sample.interaction);
					
					// if the emitter is delta, the weight should be 1
					// f(i) * g(i) * w(i) / (p(i) * nf) + f(j) * g(j) * w(j) / (p(j) * ng)
					// f is the phase function, g is the emitter, p is the pdf
					// nf and ng is the number of samples
					// weight = (nf * f)^2 / (ng * g)^2 = (nf * f / all)^2 / (ng * g / all)^2
					// all = nf + ng
					const auto weight = emitter->component<emitters::emitter>()->is_delta() ? 1 :
						power_heuristic(emitter_sample.pdf, function_pdf);

					L += function_value * emitter_sample.intensity * beam * weight / emitter_sample.pdf;
				}
			}
		}
	}

	{
		const auto function_sample = interaction.function->sample(interaction, samplers.sampler2d->next());
		const auto function_value = spectrum(function_sample.value);
		const auto function_pdf = function_sample.value;
		
		if (!function_value.is_black() && function_pdf > 0) {
			// find the emitter the sample ray intersect
			auto [emitter_interaction, pdf] = find_emitter(scene, samplers, interaction, function_sample.wi);

			if (emitter_interaction.has_value() && pdf > 0) {

				const auto emitter = emitter_interaction->entity;
				const auto emitter_value = emitter->evaluate<emitters::emitter>(emitter_interaction.value(), -function_sample.wi);
				const auto emitter_pdf = emitter->pdf<emitters::emitter>(interaction, function_sample.wi) * pdf;

				if (!emitter_value.is_black() && emitter_pdf > 0) {
					
					// if we need handle the media, we will evaluate the media beam from surface to light
					const auto beam = scene->evaluate_media_beam(samplers.sampler1d,
						{ tracing_info.medium, interaction }, emitter_interaction.value());

					// if the bsdf is delta, the weight should be 1
					// f(i) * g(i) * w(i) / (p(i) * nf) + f(j) * g(j) * w(j) / (p(j) * ng)
					// f is the phase function, g is the emitter, p is the pdf
					// nf and ng is the number of samples
					// weight = (nf * f)^2 / (ng * g)^2 = (nf * f / all)^2 / (ng * g / all)^2
					// all = nf + ng
					const auto weight = power_heuristic(function_pdf, emitter_pdf);

					L += function_value * emitter_value * beam * weight / function_pdf;
				}
			}
		}
	}

	return L;
}

bool rainbow::cpus::integrators::sample_scattering_surface_function(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers, 
	const surface_properties& properties, path_tracing_info& tracing_info, bool media)
{
	// we sample the bssrdf to get the interaction of wi and pi
	// it sample the S_p(r) of bssrdf(the part of po and pi)
	// the first part of bssrdf was sampled by the integrator main loop
	// the last part of bssrdf will be sampled after we sampled S_p(r)
	const auto scattering_sample = properties.bssrdf->sample(scene, vector3(samplers.sampler1d->next(), samplers.sampler2d->next()));

	// if we sample bssrdf failed, we just return false to indicate the path is ended.
	// because the ray can not passed the area 
	if (scattering_sample.value.is_black() || scattering_sample.pdf == 0) return false;

	// account beta value
	tracing_info.beta *= scattering_sample.value / scattering_sample.pdf;

	// uniform sample one emitter
	tracing_info.value += tracing_info.beta * uniform_sample_one_emitter(scene, samplers, tracing_info,
		scattering_sample.interaction, scattering_sample.functions, media);

	// sample the special scattering functions to find the wi
	// this function will process the part S_w(wi)(the second fresnel part of bssrdf) of bssrdf
	const auto function_sample = scattering_sample.functions.sample(scattering_sample.interaction, samplers.sampler2d->next());

	// if we sample bssrdf failed, we just return false to indicate the path is ended. 
	if (function_sample.value.is_black() || function_sample.pdf == 0) return false;

	// account beta value
	tracing_info.beta *= function_sample.value * math::abs(dot(function_sample.wi, scattering_sample.interaction.shading_space.z())) / function_sample.pdf;

	tracing_info.specular = has(function_sample.type, scattering_type::specular);

	// create a new ray to trace
	tracing_info.ray = scattering_sample.interaction.spawn_ray(function_sample.wi);

	return true;
}

bool rainbow::cpus::integrators::sample_surface_interaction(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers, 
	const std::optional<surface_interaction>& interaction, 
	path_tracing_info& tracing_info, int& bounces, bool media)
{
	// if we do not find the shape that the ray intersect we can end this tracing
	if (!interaction.has_value()) {
		// if it is not first bounce or it is not specular bounce at last bounce
		// we just end this tracing
		// if it is first bounce or it is specular bounce, we will evaluate the environment light
		if (bounces != 0 && !tracing_info.specular) return false;

		for (const auto& environment : scene->environments())
			tracing_info.value += tracing_info.beta * environment->evaluate<emitter>(interactions::interaction(), -tracing_info.ray.direction);

		return false;
	}

	// when the first ray intersect a light, we will evaluate the intensity of it
	// when the specular ray intersect an emitter, we will evaluate the intensity of it
	// because the specular ray we do not solve it at "uniform_sample_one_emitter".
	if ((bounces == 0 || tracing_info.specular) && interaction->entity->has_component<emitter>())
		tracing_info.value += tracing_info.beta * interaction->entity->evaluate<emitter>(interaction.value(), -tracing_info.ray.direction);

	// when the material is nullptr, we can think it is a invisible entity
	// we will continue spawn a ray without changing the direction
	if (!interaction->entity->has_component<material>()) {
		// compute the new ray 
		tracing_info.ray = interaction->spawn_ray(tracing_info.ray.direction);

		// because we intersect a invisible shape, we do not need add the bounces
		bounces--;

		return true;
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
		tracing_info.value += tracing_info.beta * uniform_sample_one_emitter(
			scene, samplers, tracing_info, interaction.value(), scattering_functions, media);

	const auto scattering_sample = scattering_functions.sample(interaction.value(), samplers.sampler2d->next());

	if (scattering_sample.value.is_black() || scattering_sample.pdf == 0) return false;

	tracing_info.beta *= scattering_sample.value * math::abs(dot(scattering_sample.wi, interaction->shading_space.z())) / scattering_sample.pdf;

	// indicate the path it is specular or not.
	// if it is, we will evaluate it when we intersect an emitter at next bounce
	tracing_info.specular = has(scattering_sample.type, scattering_type::specular);

	if (has(scattering_sample.type, scattering_type::specular | scattering_type::transmission)) {
		const auto surface_eta = scattering_functions.eta();

		tracing_info.eta = tracing_info.eta * ((dot(interaction->wo, interaction->normal) > 0) ?
			(surface_eta * surface_eta) : (1 / (surface_eta * surface_eta)));
	}

	tracing_info.ray = interaction->spawn_ray(scattering_sample.wi);

	// if the bssrdf is not empty and the ray pass the surface of entity, we will sample the bssrdf
	if (surface_properties.bssrdf != nullptr && has(scattering_sample.type, scattering_type::transmission))
		return sample_scattering_surface_function(scene, samplers, surface_properties, tracing_info, media);

	return true;
}

bool rainbow::cpus::integrators::sample_medium_interaction(
	const std::shared_ptr<scene>& scene, const sampler_group& samplers, 
	const std::optional<medium_interaction>& interaction,
	path_tracing_info& tracing_info, int& bounces)
{
	tracing_info.value += tracing_info.beta * uniform_sample_one_emitter(scene, samplers, tracing_info, interaction.value());

	// sample the phase function, find the new ray to tracing
	const auto phase_sample = interaction->function->sample(interaction.value(), samplers.sampler2d->next());

	// because the pdf of phase_function is the value of it, so we do not need update the beta
	tracing_info.specular = false;
	tracing_info.ray = interaction->spawn_ray(phase_sample.wi);

	return true;
}

rainbow::core::real rainbow::cpus::integrators::power_heuristic(real f_pdf, real g_pdf)
{
	return (f_pdf * f_pdf) / (f_pdf * f_pdf + g_pdf * g_pdf);
}
