#include "matte_material.hpp"

#include "../scatterings/reflection/lambertian_reflection.hpp"
#include "../scatterings/reflection/oren_nayar_reflection.hpp"

using namespace rainbow::scatterings;

rainbow::materials::matte_material::matte_material(
	const std::shared_ptr<texture2d<spectrum>>& diffuse,
	const std::shared_ptr<texture2d<real>>& sigma) :
	mDiffuse(diffuse), mSigma(sigma)
{
}

rainbow::scattering_function_collection rainbow::materials::matte_material::build_scattering_functions(
	const surface_interaction& interaction) const noexcept
{
	scattering_function_collection functions;

	const auto reflectance = mDiffuse->sample(interaction);
	const auto sigma = clamp(mSigma->sample(interaction), static_cast<real>(0), static_cast<real>(90));

	if (reflectance.is_black()) return functions;

	if (sigma == 0)
		functions.add_scattering_function(std::make_shared<lambertian_reflection>(reflectance));
	else
		functions.add_scattering_function(std::make_shared<oren_nayar_reflection>(reflectance, sigma));

	return functions;
}
