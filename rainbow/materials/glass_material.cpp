#include "glass_material.hpp"

#include "../scatterings/distribution/trowbridge_reitz_distribution.hpp"

#include "../scatterings/reflection/microfacet_reflection.hpp"
#include "../scatterings/reflection/specular_reflection.hpp"

#include "../scatterings/transmission/microfacet_transmission.hpp"
#include "../scatterings/transmission/specular_transmission.hpp"

#include "../scatterings/mixture/fresnel_specular.hpp"

rainbow::materials::glass_material::glass_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& reflectance,
	const std::shared_ptr<textures::texture2d<spectrum>>& transmission,
	const std::shared_ptr<textures::texture2d<real>>& roughness_u,
	const std::shared_ptr<textures::texture2d<real>>& roughness_v,
	const std::shared_ptr<textures::texture2d<real>>& eta,
	bool map_roughness_to_alpha) :
	mReflectance(reflectance), mTransmission(transmission), mRoughnessU(roughness_u), mRoughnessV(roughness_v),
	mEta(eta), mMapRoughnessToAlpha(map_roughness_to_alpha)
{
}

rainbow::materials::surface_properties rainbow::materials::glass_material::build_surface_properties(
	const surface_interaction& interaction) const noexcept
{
	const auto eta = mEta->sample(interaction);
	const auto roughness_u = mRoughnessU->sample(interaction);
	const auto roughness_v = mRoughnessV->sample(interaction);
	const auto reflectance = mReflectance->sample(interaction);
	const auto transmission = mTransmission->sample(interaction);

	surface_properties properties;

	properties.functions = scattering_function_collection(eta);
	
	if (reflectance.is_black() && transmission.is_black()) return properties;

	const auto is_specular = roughness_u == 0 && roughness_v == 0;

	std::shared_ptr<microfacet_distribution> distribution =
		is_specular ? nullptr : std::make_shared<trowbridge_reitz_distribution>(
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_u) : roughness_u,
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_v) : roughness_v,
			true
			);

	if (is_specular) {
		properties.functions.add_scattering_function(std::make_shared<fresnel_specular>(transmission, reflectance, static_cast<real>(1), eta));
	}

	if (!reflectance.is_black() && !is_specular) {
		const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1), eta);

		properties.functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, reflectance));
	}

	if (!transmission.is_black() && !is_specular) {
		properties.functions.add_scattering_function(std::make_shared<microfacet_transmission>(distribution, transmission, static_cast<real>(1), eta));
	}

	return properties;
}

rainbow::materials::surface_properties rainbow::materials::glass_material::build_surface_properties(
	const surface_interaction& interaction, const spectrum& scale) const noexcept
{
	const auto eta = mEta->sample(interaction);
	const auto roughness_u = mRoughnessU->sample(interaction);
	const auto roughness_v = mRoughnessV->sample(interaction);
	const auto reflectance = mReflectance->sample(interaction) * scale;
	const auto transmission = mTransmission->sample(interaction) * scale;

	surface_properties properties;

	properties.functions = scattering_function_collection(eta);
	
	if (reflectance.is_black() && transmission.is_black()) return properties;

	const auto is_specular = roughness_u == 0 && roughness_v == 0;

	std::shared_ptr<microfacet_distribution> distribution =
		is_specular ? nullptr : std::make_shared<trowbridge_reitz_distribution>(
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_u) : roughness_u,
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_v) : roughness_v,
			true
			);

	if (!reflectance.is_black()) {
		const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1), eta);

		if (is_specular)
			properties.functions.add_scattering_function(std::make_shared<specular_reflection>(fresnel, reflectance));
		else
			properties.functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, reflectance));
	}

	if (!transmission.is_black()) {
		if (is_specular)
			properties.functions.add_scattering_function(std::make_shared<specular_transmission>(transmission, static_cast<real>(1), eta));
		else
			properties.functions.add_scattering_function(std::make_shared<microfacet_transmission>(distribution, transmission, static_cast<real>(1), eta));
	}

	return properties;
}