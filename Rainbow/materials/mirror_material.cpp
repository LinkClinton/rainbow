#include "mirror_material.hpp"

#include "../scatterings/reflection/specular_reflection.hpp"

using namespace rainbow::scatterings;

rainbow::materials::mirror_material::mirror_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& reflectance) :
	mReflectance(reflectance)
{
}

rainbow::scattering_function_collection rainbow::materials::mirror_material::build_scattering_functions(
	const surface_interaction& interaction) const noexcept
{
	scattering_function_collection functions;

	const auto reflectance = mReflectance->sample(interaction);

	if (reflectance.is_black()) return functions;

	functions.add_scattering_function(std::make_shared<specular_reflection>(
		std::make_shared<fresnel_effect_nop>(),
		reflectance
		));

	return functions;
}
