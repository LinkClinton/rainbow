#include "matte_material.hpp"

#include "../scatterings/reflection/lambertian_reflection.hpp"
#include "../scatterings/reflection/oren_nayar_reflection.hpp"

rainbow::cpus::materials::matte_material::matte_material(
	const std::shared_ptr<texture2d<spectrum>>& diffuse,
	const std::shared_ptr<texture2d<real>>& sigma) :
	mDiffuse(diffuse), mSigma(sigma)
{
}

rainbow::cpus::materials::surface_properties rainbow::cpus::materials::matte_material::build_surface_properties(
	const surface_interaction& interaction) const noexcept
{
	const auto diffuse = mDiffuse->sample(interaction);
	const auto sigma = math::clamp(mSigma->sample(interaction), static_cast<real>(0), static_cast<real>(90));

	surface_properties properties;
	
	if (diffuse.is_black()) return properties;

	if (sigma == 0)
		properties.functions.add_scattering_function(std::make_shared<lambertian_reflection>(diffuse));
	else
		properties.functions.add_scattering_function(std::make_shared<oren_nayar_reflection>(diffuse, sigma));

	return properties;
}

rainbow::cpus::materials::surface_properties rainbow::cpus::materials::matte_material::build_surface_properties(
	const surface_interaction& interaction, const spectrum& scale) const noexcept
{
	const auto diffuse = mDiffuse->sample(interaction) * scale;
	const auto sigma = math::clamp(mSigma->sample(interaction), static_cast<real>(0), static_cast<real>(90));

	surface_properties properties;
	
	if (diffuse.is_black()) return properties;

	if (sigma == 0)
		properties.functions.add_scattering_function(std::make_shared<lambertian_reflection>(diffuse));
	else
		properties.functions.add_scattering_function(std::make_shared<oren_nayar_reflection>(diffuse, sigma));

	return properties;
}