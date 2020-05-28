#include "translucent_material.hpp"

#include "../scatterings/distribution/trowbridge_reitz_distribution.hpp"

#include "../scatterings/transmission/lambertian_transmission.hpp"
#include "../scatterings/transmission/microfacet_transmission.hpp"

#include "../scatterings/reflection/lambertian_reflection.hpp"
#include "../scatterings/reflection/microfacet_reflection.hpp"

rainbow::materials::translucent_material::translucent_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& transmission,
	const std::shared_ptr<textures::texture2d<spectrum>>& reflectance,
	const std::shared_ptr<textures::texture2d<spectrum>>& specular,
	const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
	const std::shared_ptr<textures::texture2d<real>>& roughness, 
	bool map_roughness_to_alpha) :
	mTransmission(transmission), mReflectance(reflectance), mSpecular(specular), mDiffuse(diffuse),
	mRoughness(roughness), mMapRoughnessToAlpha(map_roughness_to_alpha)
{
}

rainbow::scattering_function_collection rainbow::materials::translucent_material::build_scattering_functions(
	const surface_interaction& interaction) const noexcept
{
	const auto transmission = mTransmission->sample(interaction);
	const auto reflectance = mReflectance->sample(interaction);
	const auto specular = mSpecular->sample(interaction);
	const auto diffuse = mDiffuse->sample(interaction);
	const auto roughness = mRoughness->sample(interaction);
	const auto eta = static_cast<real>(1.5);

	scattering_function_collection functions(eta);

	if (reflectance.is_black() && transmission.is_black()) return functions;

	if (!diffuse.is_black()) {
		if (!reflectance.is_black())
			functions.add_scattering_function(std::make_shared<lambertian_reflection>(diffuse * reflectance));
		if (!transmission.is_black())
			functions.add_scattering_function(std::make_shared<lambertian_transmission>(diffuse * transmission));
	}

	if (!specular.is_black()) {

		const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness) : roughness,
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness) : roughness,
			true);

		if (!reflectance.is_black()) {
			const auto fresnel = std::make_shared<fresnel_effect_dielectric>(static_cast<real>(1), eta);

			functions.add_scattering_function(std::make_shared<microfacet_reflection>(distribution, fresnel, specular * reflectance));
		}

		if (!transmission.is_black()) {
			functions.add_scattering_function(std::make_shared<microfacet_transmission>(distribution, specular * transmission, 
				static_cast<real>(1), eta));
		}
	}
	
	return functions;
}
