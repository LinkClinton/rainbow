#include "plastic_material.hpp"

#include "../scatterings/distribution/trowbridge_reitz_distribution.hpp"
#include "../scatterings/reflection/lambertian_reflection.hpp"
#include "../scatterings/reflection/microfacet_reflection.hpp"

rainbow::materials::plastic_material::plastic_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& specular,
	const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
	const std::shared_ptr<textures::texture2d<real>>& roughness) :
	mSpecular(specular), mDiffuse(diffuse), mRoughness(roughness)
{
}

rainbow::scattering_function_collection rainbow::materials::plastic_material::build_scattering_functions(
	const surface_interaction& interaction) const noexcept
{
	scattering_function_collection functions;

	const auto specular = mSpecular->sample(interaction);
	const auto diffuse = mDiffuse->sample(interaction);
	const auto roughness = mRoughness->sample(interaction);

	if (!diffuse.is_black())
		functions.add_scattering_function(std::make_shared<lambertian_reflection>(diffuse));

	if (!specular.is_black()) {
		const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
			trowbridge_reitz_distribution::roughness_to_alpha(roughness), 
			trowbridge_reitz_distribution::roughness_to_alpha(roughness),
			true);
		const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1.5), static_cast<real>(1));

		functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, specular));
	}

	return functions;
}
