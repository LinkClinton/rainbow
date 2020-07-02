#include "direct_integrator.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::integrators::direct_integrator::direct_integrator(
	const std::shared_ptr<sampler2d>& sampler2d,
	const std::shared_ptr<sampler1d>& sampler1d, 
	size_t emitter_samples, size_t bsdf_samples) : sampler_integrator(sampler2d),
	mSampler1D(sampler1d), mEmitterSamples(emitter_samples), mBSDFSamples(bsdf_samples)
{
	const auto sum_samples = mEmitterSamples + mBSDFSamples;

	mWeightEmitterSamples = static_cast<real>(1) / mEmitterSamples;
	mWeightBSDFSamples = static_cast<real>(1) / mBSDFSamples;

	mFractionalEmitterSamples = static_cast<real>(mEmitterSamples) / sum_samples;
	mFractionalBSDFSamples = static_cast<real>(mBSDFSamples) / sum_samples;
}

spectrum rainbow::cpus::integrators::direct_integrator::trace(
	const std::shared_ptr<scene>& scene,
	const integrator_debug_info& debug, 
	const sampler_group& samplers, 
	const ray& ray, size_t depth)
{
	// if there are no emitters, we do not trace the ray. just return 0.
	if (scene->emitters().empty()) return spectrum(0);
	
	spectrum L = 0;

	const auto interaction = scene->intersect(ray);
	
	// when the ray was not intersect any shapes
	// we will think it intersect the environment emitter
	if (!interaction.has_value()) {
		for (const auto& environment : scene->environments())
			L += environment->evaluate<emitter>(interactions::interaction(), -ray.direction);

		return L;
	}

	// if we intersect a entity with emitter we need evaluate the emitter
	if (interaction->entity->has_component<emitter>()) 
		L += interaction->entity->evaluate<emitter>(interaction.value(), -ray.direction);

	// get the surface properties from material which the ray intersect
	// if the entity does not have material, we return default surface properties(0 functions)
	const auto surface_properties =
		interaction->entity->has_component<material>() ?
		interaction->entity->component<material>()->build_surface_properties(interaction.value()) :
		materials::surface_properties();
	
	const auto& scattering_functions = surface_properties.functions;
	
	// when the scattering functions is empty, we can think it is a invisible entity
	// we will continue spawn a ray without changing the direction
	if (scattering_functions.count() == 0)
		return L + trace(scene, debug, samplers, interaction->spawn_ray(ray.direction), depth);
	
	// emitter sampling, we sample the emitters with multiple important sampling

	for (size_t index = 0; index < mEmitterSamples; index++) {
		// sample which emitter we will sample 
		auto [emitter, pdf] = uniform_sample_one_emitter(scene, samplers);

		// sample where the light spawn
		auto emitter_sample = emitter->sample<emitters::emitter>(interaction.value(), samplers.sampler2d->next());

		// the real pdf of emitter_sample should multi the pdf of sampling which emitter
		emitter_sample.pdf = emitter_sample.pdf * pdf;

		if (!emitter_sample.intensity.is_black() && emitter_sample.pdf > 0) {
			const auto wi = interaction->from_world_to_space(emitter_sample.wi);
			const auto wo = interaction->from_world_to_space(interaction->wo);

			auto function_value = scattering_functions.evaluate(wo, wi);
			auto function_pdf = scattering_functions.pdf(wo, wi);

			function_value = function_value * math::abs(dot(emitter_sample.wi, interaction->shading_space.z()));

			if (function_value.is_black() || function_pdf <= 0) continue;

			const auto shadow_ray = interaction->spawn_ray_to(emitter_sample.interaction.point);
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
					power_heuristic(emitter_sample.pdf * mFractionalEmitterSamples, function_pdf * mFractionalBSDFSamples) * 
					mWeightEmitterSamples;

				L += function_value * emitter_sample.intensity * weight / emitter_sample.pdf;
			}
		}
	}

	// bsdf sampling, we sample the bsdfs with multiple important sampling

	for (size_t index = 0; index < mBSDFSamples; index++) {
		auto function_sample = scattering_functions.sample(interaction.value(), samplers.sampler2d->next());

		function_sample.value = function_sample.value * math::abs(dot(function_sample.wi, interaction->shading_space.z()));

		if (!function_sample.value.is_black() && function_sample.pdf > 0) {
			// find the emitter the sample ray intersect
			auto [emitter_interaction, pdf] = find_emitter(scene, samplers, interaction.value(), function_sample.wi);

			if (emitter_interaction.has_value() && pdf > 0) {

				const auto emitter = emitter_interaction->entity;
				const auto emitter_value = emitter->evaluate<emitters::emitter>(emitter_interaction.value(), -function_sample.wi);
				const auto emitter_pdf = emitter->pdf<emitters::emitter>(interaction.value(), function_sample.wi) * pdf;

				if (emitter_value.is_black() || emitter_pdf <= 0) continue;
				
				// if the bsdf is delta, the weight should be 1
				// f(i) * g(i) * w(i) / (p(i) * nf) + f(j) * g(j) * w(j) / (p(j) * ng)
				// f is the scattering functions, g is the emitter, p is the pdf
				// nf and ng is the number of samples
				// weight = (nf * f)^2 / (ng * g)^2 = (nf * f / all)^2 / (ng * g / all)^2
				// all = nf + ng
				const auto weight = has(function_sample.type, scattering_type::specular) ? 1 :
					power_heuristic(function_sample.pdf * mFractionalBSDFSamples, emitter_pdf * mFractionalEmitterSamples) *
					mWeightBSDFSamples;

				L += function_sample.value * emitter_value * weight / function_sample.pdf;
			}
		}
	}

	return L;
}

rainbow::cpus::integrators::sampler_group rainbow::cpus::integrators::direct_integrator::prepare_samplers(uint64 seed)
{
	const auto generator = std::make_shared<random_generator>(seed);

	return sampler_group(
		mSampler1D->clone(generator),
		mSampler2D->clone(generator)
	);
}
