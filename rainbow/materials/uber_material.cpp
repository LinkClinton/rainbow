#include "uber_material.hpp"

#include "../scatterings/distribution/trowbridge_reitz_distribution.hpp"

#include "../scatterings/reflection/lambertian_reflection.hpp"

#include "../scatterings/reflection/microfacet_reflection.hpp"
#include "../scatterings/reflection/specular_reflection.hpp"

#include "../scatterings/transmission/microfacet_transmission.hpp"
#include "../scatterings/transmission/specular_transmission.hpp"

rainbow::materials::uber_material::uber_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& transmission,
	const std::shared_ptr<textures::texture2d<spectrum>>& reflectance,
	const std::shared_ptr<textures::texture2d<spectrum>>& specular,
	const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
	const std::shared_ptr<textures::texture2d<spectrum>>& opacity,
	const std::shared_ptr<textures::texture2d<real>>& roughness_u,
	const std::shared_ptr<textures::texture2d<real>>& roughness_v,
	const std::shared_ptr<textures::texture2d<real>>& eta,
	bool map_roughness_to_alpha) :
	mTransmission(transmission), mReflectance(reflectance), mSpecular(specular), mDiffuse(diffuse),
	mOpacity(opacity), mRoughnessU(roughness_u), mRoughnessV(roughness_v), mEta(eta), mMapRoughnessToAlpha(map_roughness_to_alpha)
{
}

rainbow::materials::surface_properties rainbow::materials::uber_material::build_surface_properties(
	const surface_interaction& interaction) const noexcept
{
	const auto eta = mEta->sample(interaction);
	const auto opacity = mOpacity->sample(interaction);
	const auto roughness_u = mRoughnessU->sample(interaction);
	const auto roughness_v = mRoughnessV->sample(interaction);
	const auto reflectance = opacity * mReflectance->sample(interaction);
	const auto transmission = opacity * mTransmission->sample(interaction);
	const auto specular = opacity * mSpecular->sample(interaction);
	const auto diffuse = opacity * mDiffuse->sample(interaction);

	surface_properties properties;

	properties.functions = scattering_function_collection(eta);
	
	const auto invert = clamp(spectrum(1) - opacity);

	if (!invert.is_black())
		properties.functions.add_scattering_function(std::make_shared<specular_transmission>(invert, static_cast<real>(1), static_cast<real>(1)));

	if (!diffuse.is_black())
		properties.functions.add_scattering_function(std::make_shared<lambertian_reflection>(diffuse));

	if (!specular.is_black()) {
		const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_u) : roughness_u,
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_v) : roughness_v,
			true);
		const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1), eta);

		properties.functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, specular));
	}

	if (!reflectance.is_black()) {
		const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1), eta);

		properties.functions.add_scattering_function(std::make_shared<specular_reflection>(fresnel, reflectance));
	}

	if (!transmission.is_black())
		properties.functions.add_scattering_function(std::make_shared<specular_transmission>(transmission, static_cast<real>(1), eta));

	return properties;
}

rainbow::materials::surface_properties rainbow::materials::uber_material::build_surface_properties(
	const surface_interaction& interaction, const spectrum& scale) const noexcept
{
	const auto eta = mEta->sample(interaction);
	const auto opacity = mOpacity->sample(interaction);
	const auto roughness_u = mRoughnessU->sample(interaction);
	const auto roughness_v = mRoughnessV->sample(interaction);
	const auto reflectance = opacity * mReflectance->sample(interaction);
	const auto transmission = opacity * mTransmission->sample(interaction);
	const auto specular = opacity * mSpecular->sample(interaction);
	const auto diffuse = opacity * mDiffuse->sample(interaction);

	surface_properties properties;

	properties.functions = scattering_function_collection(eta);
	
	const auto invert = clamp(spectrum(1) - opacity);

	if (!invert.is_black())
		properties.functions.add_scattering_function(std::make_shared<specular_transmission>(invert * scale, static_cast<real>(1), static_cast<real>(1)));

	if (!diffuse.is_black())
		properties.functions.add_scattering_function(std::make_shared<lambertian_reflection>(diffuse * scale));

	if (!specular.is_black()) {
		const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_u) : roughness_u,
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_v) : roughness_v,
			true);
		const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1), eta);

		properties.functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, specular * scale));
	}

	if (!reflectance.is_black()) {
		const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1), eta);

		properties.functions.add_scattering_function(std::make_shared<specular_reflection>(fresnel, reflectance * scale));
	}

	if (!transmission.is_black())
		properties.functions.add_scattering_function(std::make_shared<specular_transmission>(transmission * scale, static_cast<real>(1), eta));

	return properties;
}