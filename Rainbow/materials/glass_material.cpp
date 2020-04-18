#include "glass_material.hpp"

#include "../scatterings/distribution/trowbridge_reitz_distribution.hpp"

#include "../scatterings/reflection/microfacet_reflection.hpp"
#include "../scatterings/reflection/specular_reflection.hpp"

#include "../scatterings/transmission/microfacet_transmission.hpp"
#include "../scatterings/transmission/specular_transmission.hpp"

rainbow::materials::glass_material::glass_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& reflectance,
	const std::shared_ptr<textures::texture2d<spectrum>>& transmission,
	const std::shared_ptr<textures::texture2d<vector2>>& roughness,
	const std::shared_ptr<textures::texture2d<real>>& eta) :
	mReflectance(reflectance), mTransmission(transmission), mRoughness(roughness), mEta(eta)
{
}

rainbow::scattering_function_collection rainbow::materials::glass_material::build_scattering_functions(
	const surface_interaction& interaction) const noexcept
{
	scattering_function_collection functions;
	
	const auto eta = mEta->sample(interaction);
	const auto roughness = mRoughness->sample(interaction);
	const auto reflectance = mReflectance->sample(interaction);
	const auto transmission = mTransmission->sample(interaction);

	if (reflectance.is_black() && transmission.is_black()) return functions;

	const auto is_specular = roughness == vector2(0);

	std::shared_ptr<microfacet_distribution> distribution =
		is_specular ? nullptr : std::make_shared<trowbridge_reitz_distribution>(
			trowbridge_reitz_distribution::roughness_to_alpha(roughness.x),
			trowbridge_reitz_distribution::roughness_to_alpha(roughness.y),
			true
			);
	
	if (!reflectance.is_black()) {
		const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1), eta);

		if (is_specular)
			functions.add_scattering_function(std::make_shared<specular_reflection>(fresnel, reflectance));
		else
			functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, reflectance));
	}

	if (!transmission.is_black()) {
		if (is_specular)
			functions.add_scattering_function(std::make_shared<specular_transmission>(transmission, static_cast<real>(1), eta));
		else
			functions.add_scattering_function(std::make_shared<microfacet_transmission>(distribution, transmission, static_cast<real>(1), eta));
	}

	return functions;
}
