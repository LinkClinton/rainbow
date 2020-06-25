#include "mixture_material.hpp"

#define MATERIAL_BUILD_IF_NOT_NULL(material, interaction, scale) (material == nullptr ? \
	scattering_function_collection() : \
	material->build_scattering_functions(interaction, scale))

rainbow::cpus::materials::mixture_material::mixture_material(const std::shared_ptr<textures::texture2d<spectrum>> alpha,
	const std::shared_ptr<material>& material0, const std::shared_ptr<material>& material1) :
	mAlpha(alpha), mMaterials({ material0, material1 })
{
}

rainbow::cpus::materials::surface_properties rainbow::cpus::materials::mixture_material::build_surface_properties(
	const surface_interaction& interaction, const transport_mode& mode) const noexcept
{
	const auto alpha0 = mAlpha->sample(interaction);
	const auto alpha1 = clamp(spectrum(1) - alpha0);

	surface_properties properties;

	// mixture material is not support bssrdf
	const auto functions0 = mMaterials[0]->build_surface_properties(interaction, alpha0, mode).functions;
	const auto functions1 = mMaterials[1]->build_surface_properties(interaction, alpha1, mode).functions;

	properties.functions = scattering_function_collection(functions0, functions1);

	return properties;
}

rainbow::cpus::materials::surface_properties rainbow::cpus::materials::mixture_material::build_surface_properties(
	const surface_interaction& interaction, const spectrum& scale, const transport_mode& mode) const noexcept
{
	const auto alpha0 = mAlpha->sample(interaction);
	const auto alpha1 = clamp(spectrum(1) - alpha0);

	surface_properties properties;
	
	const auto functions0 = mMaterials[0]->build_surface_properties(interaction, alpha0 * scale, mode).functions;
	const auto functions1 = mMaterials[1]->build_surface_properties(interaction, alpha1 * scale, mode).functions;

	properties.functions = scattering_function_collection(functions0, functions1);

	return properties;
}