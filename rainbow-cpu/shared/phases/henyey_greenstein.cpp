#include "henyey_greenstein.hpp"

#include "../../../rainbow-core/shading_function.hpp"

#include "../coordinate_system.hpp"

rainbow::cpus::shared::phases::henyey_greenstein::henyey_greenstein(real g) : mG(g)
{
}

rainbow::core::real rainbow::cpus::shared::phases::henyey_greenstein::evaluate(const vector3& wo,
	const vector3& wi) const
{
	return henyey_greenstein_phase_function(dot(wo, wi), mG);
}

rainbow::cpus::shared::phases::phase_sample rainbow::cpus::shared::phases::henyey_greenstein::sample(
	const interaction& interaction, const vector2& sample) const
{
	// for phase function, we sample theta and phi and use them to build sphere direction
	real cos_theta;

	// for theta, we have
	// for g == 0, cos_theta = 1 - 2 * sample
	// for g != 0, cos_theta = (1 + g^2 - ((1 - g^2) / (1 - g + 2 * g * sample))^2) / (2 * g)
	if (abs(mG) >= 1e-3) {
		const auto sqr_term = (1 - mG * mG) / (1 - mG + 2 * mG * sample.x);

		cos_theta = (1 + mG * mG - sqr_term * sqr_term) / (2 * mG);
	}
	else cos_theta = 1 - 2 * sample.x;

	// for phi, we just sample it from two_pi. phi = two_pi * sample
	// when we get theta and phi, we can build the wi at local space
	// and then transform wi to world space
	const auto sin_theta = sqrt(max(static_cast<real>(0), 1 - cos_theta * cos_theta));
	const auto phi = two_pi<real>() * sample.y;
	const auto wi = spherical_direction(sin_theta, cos_theta, phi);

	auto system = coordinate_system(interaction.wo);

	system.z() = -interaction.wo;

	return phase_sample(
		local_to_world(system, wi),
		henyey_greenstein_phase_function(-cos_theta, mG)
	);
}

