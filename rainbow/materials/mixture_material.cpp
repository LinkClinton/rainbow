#include "mixture_material.hpp"

#define MATERIAL_BUILD_IF_NOT_NULL(material, interaction, scale) (material == nullptr ? \
	scattering_function_collection() : \
	material->build_scattering_functions(interaction, scale))

rainbow::materials::mixture_material::mixture_material(const std::shared_ptr<textures::texture2d<spectrum>> alpha,
	const std::shared_ptr<material>& material0, const std::shared_ptr<material>& material1) :
	mAlpha(alpha), mMaterials({ material0, material1 })
{
}

rainbow::scattering_function_collection rainbow::materials::mixture_material::build_scattering_functions(
	const surface_interaction& interaction) const noexcept
{
	const auto alpha0 = mAlpha->sample(interaction);
	const auto alpha1 = clamp(spectrum(1) - alpha0);

	const auto functions0 = mMaterials[0]->build_scattering_functions(interaction, alpha0);
	const auto functions1 = mMaterials[1]->build_scattering_functions(interaction, alpha1);

	return scattering_function_collection(functions0, functions1);
}

rainbow::scattering_function_collection rainbow::materials::mixture_material::build_scattering_functions(
	const surface_interaction& interaction, const spectrum& scale) const noexcept
{
	const auto alpha0 = mAlpha->sample(interaction);
	const auto alpha1 = clamp(spectrum(1) - alpha0);

	const auto functions0 = mMaterials[0]->build_scattering_functions(interaction, alpha0 * scale);
	const auto functions1 = mMaterials[1]->build_scattering_functions(interaction, alpha1 * scale);

	return scattering_function_collection(functions0, functions1);
}
