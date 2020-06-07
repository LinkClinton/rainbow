#include "substrate_material.hpp"

#include "../scatterings/distribution/trowbridge_reitz_distribution.hpp"
#include "../scatterings/reflection/fresnel_blend_reflection.hpp"

rainbow::cpus::materials::substrate_material::substrate_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& specular,
	const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
	const std::shared_ptr<textures::texture2d<real>>& roughness_u,
	const std::shared_ptr<textures::texture2d<real>>& roughness_v,
	bool map_roughness_to_alpha) : mSpecular(specular), mDiffuse(diffuse), mRoughnessU(roughness_u), mRoughnessV(roughness_v),
	mMapRoughnessToAlpha(map_roughness_to_alpha)
{
}

rainbow::cpus::materials::surface_properties rainbow::cpus::materials::substrate_material::build_surface_properties(
	const surface_interaction& interaction) const noexcept
{
	const auto specular = mSpecular->sample(interaction);
	const auto diffuse = mDiffuse->sample(interaction);
	const auto roughness_u = mRoughnessU->sample(interaction);
	const auto roughness_v = mRoughnessV->sample(interaction);

	surface_properties properties;
	
	if (!specular.is_black() || !diffuse.is_black()) {
		const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_u) : roughness_u,
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_v) : roughness_v,
			true);

		properties.functions.add_scattering_function(std::make_shared<fresnel_blend_reflection>(distribution, specular, diffuse));
	}

	return properties;
}

rainbow::cpus::materials::surface_properties rainbow::cpus::materials::substrate_material::build_surface_properties(
	const surface_interaction& interaction, const spectrum& scale) const noexcept
{
	const auto specular = mSpecular->sample(interaction);
	const auto diffuse = mDiffuse->sample(interaction);
	const auto roughness_u = mRoughnessU->sample(interaction);
	const auto roughness_v = mRoughnessV->sample(interaction);

	surface_properties properties;

	if (!specular.is_black() || !diffuse.is_black()) {
		const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_u) : roughness_u,
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_v) : roughness_v,
			true);

		properties.functions.add_scattering_function(std::make_shared<fresnel_blend_reflection>(distribution, specular, diffuse, scale));
	}

	return properties;
}