#include "mirror_material.hpp"

#include "../scatterings/reflection/specular_reflection.hpp"

using namespace rainbow::scatterings;

rainbow::materials::mirror_material::mirror_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& reflectance) :
	mReflectance(reflectance)
{
}

rainbow::materials::surface_properties rainbow::materials::mirror_material::build_surface_properties(
	const surface_interaction& interaction) const noexcept
{
	const auto reflectance = mReflectance->sample(interaction);

	surface_properties properties;
	
	if (reflectance.is_black()) return properties;

	properties.functions.add_scattering_function(std::make_shared<specular_reflection>(
		std::make_shared<fresnel_effect_nop>(),
		reflectance
		));

	return properties;
}

rainbow::materials::surface_properties rainbow::materials::mirror_material::build_surface_properties(
	const surface_interaction& interaction, const spectrum& scale) const noexcept
{

	const auto reflectance = mReflectance->sample(interaction) * scale;

	surface_properties properties;
	
	if (reflectance.is_black()) return properties;

	properties.functions.add_scattering_function(std::make_shared<specular_reflection>(
		std::make_shared<fresnel_effect_nop>(),
		reflectance
		));

	return properties;
}