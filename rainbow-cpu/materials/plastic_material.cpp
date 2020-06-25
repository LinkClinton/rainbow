#include "plastic_material.hpp"

#include "../scatterings/distribution/trowbridge_reitz_distribution.hpp"
#include "../scatterings/reflection/lambertian_reflection.hpp"
#include "../scatterings/reflection/microfacet_reflection.hpp"

#include "../textures/constant_texture.hpp"

rainbow::cpus::materials::plastic_material::plastic_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& specular,
	const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
	const std::shared_ptr<textures::texture2d<real>>& roughness,
	bool map_roughness_to_alpha) :
	mSpecular(specular), mDiffuse(diffuse), mRoughness(roughness),
	mEta(std::make_shared<textures::constant_texture2d<real>>(static_cast<real>(1.49f))),
	mMapRoughnessToAlpha(map_roughness_to_alpha)
{
}

rainbow::cpus::materials::plastic_material::plastic_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& specular,
	const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
	const std::shared_ptr<textures::texture2d<real>>& roughness, 
	const std::shared_ptr<textures::texture2d<real>>& eta,
	bool map_roughness_to_alpha) :
	mSpecular(specular), mDiffuse(diffuse), mRoughness(roughness), mEta(eta), mMapRoughnessToAlpha(map_roughness_to_alpha)
{
}

rainbow::cpus::materials::surface_properties rainbow::cpus::materials::plastic_material::build_surface_properties(
	const surface_interaction& interaction, const transport_mode& mode) const noexcept
{
	const auto specular = mSpecular->sample(interaction);
	const auto diffuse = mDiffuse->sample(interaction);
	const auto roughness = mRoughness->sample(interaction);
	const auto eta = mEta->sample(interaction);

	surface_properties properties;
	
	properties.functions = scattering_function_collection(eta);
	
	if (!diffuse.is_black())
		properties.functions.add_scattering_function(std::make_shared<lambertian_reflection>(diffuse));

	if (!specular.is_black()) {
		const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness) : roughness,
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness) : roughness,
			true);
		const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1), eta);

		properties.functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, specular));
	}

	return properties;
}

rainbow::cpus::materials::surface_properties rainbow::cpus::materials::plastic_material::build_surface_properties(
	const surface_interaction& interaction, const spectrum& scale, const transport_mode& mode) const noexcept
{
	const auto specular = mSpecular->sample(interaction) * scale;
	const auto diffuse = mDiffuse->sample(interaction) * scale;
	const auto roughness = mRoughness->sample(interaction);
	const auto eta = mEta->sample(interaction);

	surface_properties properties;

	properties.functions = scattering_function_collection(eta);
	
	if (!diffuse.is_black())
		properties.functions.add_scattering_function(std::make_shared<lambertian_reflection>(diffuse));

	if (!specular.is_black()) {
		const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness) : roughness,
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness) : roughness,
			true);
		const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1), eta);

		properties.functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, specular));
	}

	return properties;
}