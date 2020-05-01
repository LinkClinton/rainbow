#include "scattering_function_collection.hpp"

#include "../shared/logs/log.hpp"

rainbow::scatterings::scattering_function_collection::scattering_function_collection(
	const std::vector<std::shared_ptr<scattering_function>>& functions, real eta) : mScatteringFunctions(functions), mEta(eta)
{
}

rainbow::scatterings::scattering_function_collection::scattering_function_collection(real eta) : mEta(eta)
{
}

void rainbow::scatterings::scattering_function_collection::add_scattering_function(
	const std::shared_ptr<scattering_function>& function)
{
	mScatteringFunctions.push_back(function);
}

rainbow::spectrum rainbow::scatterings::scattering_function_collection::evaluate(
	const vector3& wo, const vector3& wi, const scattering_type& include) const
{
	if (wo.z == 0) return 0;

	// get the scattering type, if the wo and wi are in the same hemisphere, it is reflection.
	const auto type = same_hemisphere(wo, wi) ? scattering_type::reflection : scattering_type::transmission;

	spectrum f = 0;

	for (const auto& function : mScatteringFunctions)
		if (scatterings::match(include, function->type()) &&
			scatterings::match(function->type(), type))
			f += function->evaluate(wo, wi);

	return f;
}

rainbow::scatterings::scattering_sample rainbow::scatterings::scattering_function_collection::sample(
	const surface_interaction& interaction, const vector2& sample, const scattering_type& include) const
{
	const auto functions = match(include);

	if (functions.empty()) return {};

	// use the sample.x to get which scattering function we will sample
	const auto which = std::min(static_cast<size_t>(std::floor(sample.x * functions.size())),
		functions.size() - 1);

	// remapped the sample, because we use the first to get function.
	// the sample_remapped.x is sample.x * functions.size() - which
	const auto sample_remapped = vector2(
		std::min(sample.x * functions.size() - which, 
			static_cast<real>(1) - std::numeric_limits<real>::epsilon()),
		sample.y);

	// transform the wo from world space to shading space
	const auto wo = interaction.from_world_to_space(interaction.wo);

	if (wo.z == 0) return {};

	// sample the function to get (wo, wi)
	auto scattering_sample = functions[which]->sample(wo, sample_remapped);

	if (scattering_sample.pdf == 0) return {};

	// if the function we sample is specular scattering, we do not need to sample other functions
	// and the value and pdf of specular scattering are 0
	// if the function we sample is not specular, we need calculate all functions(specular will return 0 of pdf and evaluate)
	if (!scatterings::match(scattering_sample.type, scattering_type::specular)) {
		const auto type = same_hemisphere(wo, scattering_sample.wi) ? scattering_type::reflection : scattering_type::transmission;

		scattering_sample.value = 0;
		scattering_sample.pdf = 0;

		// calculate the total value and pdf of functions
		for (const auto& function : functions) {
			// if it is reflection, the value and pdf of transmission functions should be 0
			// if it is transmission, the value and pdf of reflection functions should be 0
			if (scatterings::match(function->type(), type)) 
				scattering_sample.value += function->evaluate(wo, scattering_sample.wi);

			scattering_sample.pdf += function->pdf(wo, scattering_sample.wi);
		}
	}

	// the pdf of sample is the average of all functions' pdf with (wo, wi)
	scattering_sample.pdf = scattering_sample.pdf / functions.size();

	// transform the wi from shading space to world space
	scattering_sample.wi = interaction.from_space_to_world(scattering_sample.wi);
	
	return scattering_sample;
}

rainbow::spectrum rainbow::scatterings::scattering_function_collection::rho(
	const vector3& wo, const std::vector<vector2>& samples, const scattering_type& include) const
{
	spectrum spectrum = 0;

	for (const auto& function : mScatteringFunctions)
		if (scatterings::match(include, function->type()))
			spectrum += function->rho(wo, samples);

	return spectrum;
}

rainbow::spectrum rainbow::scatterings::scattering_function_collection::rho(
	const std::vector<vector2>& samples0,
	const std::vector<vector2>& samples1,
	const scattering_type& include) const
{
	spectrum spectrum = 0;

	for (const auto& function : mScatteringFunctions)
		if (scatterings::match(include, function->type()))
			spectrum += function->rho(samples0, samples1);

	return spectrum;
}

rainbow::real rainbow::scatterings::scattering_function_collection::pdf(
	const vector3& wo, const vector3& wi, const scattering_type& include) const
{
	if (count() == 0 || wo.z == 0) return 0;

	size_t count = 0;
	real pdf = 0;

	for (const auto& function : mScatteringFunctions) {
		if (scatterings::match(include, function->type())) {
			pdf = pdf + function->pdf(wo, wi);

			count++;
		}
	}

	return count != 0 ? pdf / count : 0;
}

size_t rainbow::scatterings::scattering_function_collection::count(const scattering_type& include) const noexcept
{
	size_t count = 0;
	
	for (const auto& function : mScatteringFunctions)
		if (scatterings::match(include, function->type())) count++;

	return count;
}

size_t rainbow::scatterings::scattering_function_collection::count() const noexcept
{
	return mScatteringFunctions.size();
}

rainbow::real rainbow::scatterings::scattering_function_collection::eta() const noexcept
{
	return mEta;
}

std::vector<std::shared_ptr<rainbow::scatterings::scattering_function>>
rainbow::scatterings::scattering_function_collection::match(const scattering_type& include) const noexcept
{
	std::vector<std::shared_ptr<scattering_function>> functions;

	for (const auto& function : mScatteringFunctions)
		if (scatterings::match(include, function->type())) functions.push_back(function);

	return functions;
}
