#include "subsurface_material.hpp"

#include "../scatterings/distribution/trowbridge_reitz_distribution.hpp"
#include "../scatterings/transmission/microfacet_transmission.hpp"
#include "../scatterings/reflection/microfacet_reflection.hpp"
#include "../scatterings/bssrdf/normalized_diffusion.hpp"
#include "../scatterings/mixture/fresnel_specular.hpp"


rainbow::materials::subsurface_material::subsurface_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& transmission,
	const std::shared_ptr<textures::texture2d<spectrum>>& reflectance,
	const std::shared_ptr<textures::texture2d<spectrum>>& sigma_a,
	const std::shared_ptr<textures::texture2d<spectrum>>& sigma_s,
	const std::shared_ptr<textures::texture2d<real>>& roughness_u,
	const std::shared_ptr<textures::texture2d<real>>& roughness_v,
	const std::shared_ptr<textures::texture2d<real>>& eta,
	real scale, bool map_roughness_to_alpha) :
	mTransmission(transmission), mReflectance(reflectance),
	mSigmaA(sigma_a), mSigmaS(sigma_s), mRoughnessU(roughness_u), mRoughnessV(roughness_v),
	mEta(eta), mScale(scale), mMapRoughnessToAlpha(map_roughness_to_alpha)
{
}

rainbow::materials::surface_properties rainbow::materials::subsurface_material::build_surface_properties(
	const surface_interaction& interaction) const noexcept
{
	const auto transmission = mTransmission->sample(interaction);
	const auto reflectance = mReflectance->sample(interaction);
	const auto roughness_u = mRoughnessU->sample(interaction);
	const auto roughness_v = mRoughnessV->sample(interaction);
	const auto sigma_a = mSigmaA->sample(interaction);
	const auto sigma_s = mSigmaS->sample(interaction);
	const auto eta = mEta->sample(interaction);

	surface_properties properties;

	properties.functions = scattering_function_collection(eta);

	if (transmission.is_black() || reflectance.is_black()) return properties;

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

	const auto sigma_t = sigma_a + sigma_s;
	const auto d = (sigma_t + sigma_a) / (sigma_t * sigma_t * 3);
	const auto sigma_tr = sqrt(sigma_a / d);
	
	properties.bssrdf = std::make_shared<normalized_diffusion>(interaction, reflectance, spectrum(1) / sigma_tr, eta);

	return properties;
}

rainbow::surface_properties rainbow::subsurface_material::build_surface_properties(
	const surface_interaction& interaction, const spectrum& scale) const noexcept
{
	// bssrdf is not support mixture material.
	return surface_properties();
}
