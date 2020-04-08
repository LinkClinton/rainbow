#include "scattering_function_collection.hpp"

rainbow::scatterings::scattering_function_collection::scattering_function_collection(
	const std::vector<std::shared_ptr<scattering_function>>& functions) : mScatteringFunctions(functions)
{
}

void rainbow::scatterings::scattering_function_collection::add_scattering_function(
	const std::shared_ptr<scattering_function>& function)
{
	mScatteringFunctions.push_back(function);
}

rainbow::spectrum rainbow::scatterings::scattering_function_collection::evaluate(const vector3& wo,
	const vector3& wi) const
{
	if (wo.z == 0) return 0;

	// get the scattering type, if the wo and wi are in the same hemisphere, it is reflection.
	const auto type = same_hemisphere(wo, wi) ? scattering_type::reflection : scattering_type::transmission;

	spectrum f = 0;

	for (const auto& function : mScatteringFunctions) 
		if (scatterings::match(function->type(), type)) f += function->evaluate(wo, wi);

	return f;
}

rainbow::scatterings::scattering_sample rainbow::scatterings::scattering_function_collection::sample(
	const std::shared_ptr<surface_interaction>& interaction, const vector2& sample) const
{
	const auto& functions = mScatteringFunctions;

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

	
}

rainbow::spectrum rainbow::scatterings::scattering_function_collection::rho(const vector3& wo,
                                                                            const std::vector<vector2>& samples) const
{
	spectrum spectrum = 0;

	for (const auto& function : mScatteringFunctions) 
		spectrum += function->rho(wo, samples);

	return spectrum;
}

rainbow::spectrum rainbow::scatterings::scattering_function_collection::rho(const std::vector<vector2>& samples0,
	const std::vector<vector2>& samples1) const
{
	spectrum spectrum = 0;

	for (const auto& function : mScatteringFunctions)
		spectrum += function->rho(samples0, samples1);

	return spectrum;
}

rainbow::real rainbow::scatterings::scattering_function_collection::pdf(const vector3& wo, const vector3& wi) const
{
	if (size() == 0 || wo.z == 0) return 0;

	real pdf = 0;

	for (const auto& function : mScatteringFunctions) 
		pdf = pdf + function->pdf(wo, wi);

	return pdf / size();
}

size_t rainbow::scatterings::scattering_function_collection::size() const noexcept
{
	return mScatteringFunctions.size();
}

std::vector<std::shared_ptr<rainbow::scatterings::scattering_function>>
	rainbow::scatterings::scattering_function_collection::match(const scattering_type& type) const noexcept
{
	std::vector<std::shared_ptr<scattering_function>> functions;

	for (const auto& function : mScatteringFunctions) 
		if (scatterings::match(function->type(), type)) functions.push_back(function);

	return functions;
}
