#include "metal_material.hpp"

#include "../scatterings/distribution/trowbridge_reitz_distribution.hpp"

#include "../scatterings/reflection/microfacet_reflection.hpp"

rainbow::materials::metal_material::metal_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& eta,
	const std::shared_ptr<textures::texture2d<spectrum>>& k,
	const std::shared_ptr<textures::texture2d<real>>& roughness_u,
	const std::shared_ptr<textures::texture2d<real>>& roughness_v,
	bool map_roughness_to_alpha) : mEta(eta), mK(k), mRoughnessU(roughness_u), mRoughnessV(roughness_v),
	mMapRoughnessToAlpha(map_roughness_to_alpha)
{
}

rainbow::scattering_function_collection rainbow::materials::metal_material::build_scattering_functions(
	const surface_interaction& interaction) const noexcept
{
	const auto eta = mEta->sample(interaction);
	const auto k = mK->sample(interaction);
	const auto roughness_u = mRoughnessU->sample(interaction);
	const auto roughness_v = mRoughnessV->sample(interaction);
	
	const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
		mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_u) : roughness_u,
		mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_v) : roughness_v,
		true);

	const auto fresnel = std::make_shared<fresnel_effect_conductor>(static_cast<real>(1), eta, k);

	scattering_function_collection functions;

	functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, spectrum(1)));

	return functions;
}

rainbow::scattering_function_collection rainbow::materials::metal_material::build_scattering_functions(
	const surface_interaction& interaction, const spectrum& scale) const noexcept
{
	const auto eta = mEta->sample(interaction);
	const auto k = mK->sample(interaction);
	const auto roughness_u = mRoughnessU->sample(interaction);
	const auto roughness_v = mRoughnessV->sample(interaction);

	const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
		mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_u) : roughness_u,
		mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness_v) : roughness_v,
		true);

	const auto fresnel = std::make_shared<fresnel_effect_conductor>(static_cast<real>(1), eta, k);

	scattering_function_collection functions;

	functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, scale));

	return functions;
}
