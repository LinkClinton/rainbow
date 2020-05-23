#include "substrate_material.hpp"

#include "../scatterings/distribution/trowbridge_reitz_distribution.hpp"
#include "../scatterings/reflection/fresnel_blend_reflection.hpp"

rainbow::materials::substrate_material::substrate_material(
	const std::shared_ptr<textures::texture2d<spectrum>>& specular,
	const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
	const std::shared_ptr<textures::texture2d<vector2>>& roughness, 
	bool map_roughness_to_alpha) : mSpecular(specular), mDiffuse(diffuse), mRoughness(roughness),
	mMapRoughnessToAlpha(map_roughness_to_alpha)
{
}

rainbow::scattering_function_collection rainbow::materials::substrate_material::build_scattering_functions(
	const surface_interaction& interaction) const noexcept
{
	const auto specular = mSpecular->sample(interaction);
	const auto diffuse = mDiffuse->sample(interaction);
	const auto roughness = mRoughness->sample(interaction);
	
	scattering_function_collection functions;

	if (!specular.is_black() || !diffuse.is_black()) {
		const auto distribution = std::make_shared<trowbridge_reitz_distribution>(
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness.x) : roughness.x,
			mMapRoughnessToAlpha ? trowbridge_reitz_distribution::roughness_to_alpha(roughness.y) : roughness.y,
			true);
		
		functions.add_scattering_function(std::make_shared<fresnel_blend_reflection>(distribution, specular, diffuse));
	}

	return functions;
}
