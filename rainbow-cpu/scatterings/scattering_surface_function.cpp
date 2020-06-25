#include "scattering_surface_function.hpp"

#include "reflection/separable_bssrdf_reflection.hpp"

namespace rainbow::cpus::scatterings {

	inline std::tuple<coordinate_system, size_t, real> sample_axis_and_channel(const coordinate_system& system, real sample)
	{
		coordinate_system projective_system;

		// for x-y plane(respect z-axis) we have 0.5
		if (sample < static_cast<real>(0.5)) {
			projective_system.x() = system.x();
			projective_system.y() = system.y();
			projective_system.z() = system.z();

			sample = sample * 2;
		}else if (sample < static_cast<real>(0.75)) {
			// for y-z plane(respect x-axis) we have 0.25
			
			projective_system.x() = system.y();
			projective_system.y() = system.z();
			projective_system.z() = system.x();

			sample = static_cast<real>(sample - 0.50) * 4;
		} else {
			// for x-z plane(respect y-axis) we have 0.25

			projective_system.x() = system.z();
			projective_system.y() = system.x();
			projective_system.z() = system.y();

			sample = static_cast<real>(sample - 0.75) * 4;
		}

		const auto channel = math::clamp(
			static_cast<size_t>(sample * spectrum::num_samples), static_cast<size_t>(0), spectrum::num_samples - 1);

		sample = sample * spectrum::num_samples - channel;

		return { projective_system, channel, sample };
	}
}

rainbow::cpus::scatterings::scattering_surface_sample::scattering_surface_sample(
	const scattering_function_collection& functions,
	const surface_interaction& interaction,
	const spectrum& value, real pdf) :
	functions(functions), interaction(interaction), value(value), pdf(pdf)
{
}

rainbow::cpus::scatterings::bidirectional_scattering_surface_distribution_function::bidirectional_scattering_surface_distribution_function(
	const surface_interaction& interaction, const transport_mode& mode, real eta) :
	mInteraction(interaction), mMode(mode), mEta(eta)
{
}

rainbow::cpus::scatterings::separable_bidirectional_scattering_surface_distribution_function::separable_bidirectional_scattering_surface_distribution_function(
	const surface_interaction& interaction, const transport_mode& mode, real eta)
	: bidirectional_scattering_surface_distribution_function(interaction, mode, eta),
	mMaterial(interaction.entity->component<materials::material>())
{
	mCoordinateSystem.z() = interaction.shading_space.z();
	mCoordinateSystem.x() = normalize(interaction.dp_du);
	mCoordinateSystem.y() = math::cross(mCoordinateSystem.z(), mCoordinateSystem.x());
}

rainbow::cpus::scatterings::scattering_surface_sample rainbow::cpus::scatterings::separable_bidirectional_scattering_surface_distribution_function::sample(
	const std::shared_ptr<scene>& scene, const vector3& sample)
{
	const auto [projective_system, channel, sample_remapped] = sample_axis_and_channel(mCoordinateSystem, sample.x);

	// r is the distance from wi to wo(in projective.xy space)
	// in fact, we use 2D polar coordinate (r, phi) to define the wi in projective space
	const auto r = sample_reflectance_profile(channel, sample.y);

	if (r < 0) return {};

	const auto phi = 2 * pi<real>() * sample.z;

	// max_r indicate the sphere range that contains 0.999 scattered energy
	// if the r we sampled greater than max_r, we just stop sampling
	const auto max_r = sample_reflectance_profile(channel, static_cast<real>(0.999));

	if (r >= max_r) return {};

	// l is the length the line has direction projective.z intersect with max_r sphere
	// it indicate the length of ray we need trace to find the wi
	const auto l = 2 * math::sqrt(max_r * max_r - r * r);

	surface_interaction base;

	// first, convert the (r, phi) from polar coordinate to world space
	// x = r * cos(phi), y = r * sin(phi) and transform it from projective space to world space
	// second, offset the half of l along -z direction
	// the point we start the ray is on the boundary of sphere
	// in fact, it is inside the entity the mInteraction on(because the normal indicate the outside of surface)
	base.point = mInteraction.point +
		r * (projective_system.x() * cos(phi) + projective_system.y() * sin(phi))
		- l * projective_system.z() * static_cast<real>(0.5);

	const auto target = base.point + l * projective_system.z();

	std::vector<surface_interaction> interactions;

	while (true) {
		const auto ray = base.spawn_ray_to(target);

		if (ray.direction == vector3(0)) break;

		const auto interaction = scene->intersect(ray);

		if (!interaction.has_value()) break;

		// if the entity has the same material, we think it is the object the ray entered
		if (interaction->entity->has_component<material>() && 
			interaction->entity->component<material>() == mMaterial) 
			interactions.push_back(interaction.value());
		
		base = interaction.value();
	}

	if (interactions.empty()) return {};

	// find which interaction we sampled
	const auto which = math::clamp(
		static_cast<size_t>(sample_remapped * interactions.size()),
		static_cast<size_t>(0), interactions.size() - 1);

	const auto distance = math::distance(mInteraction.point, interactions[which].point);

	scattering_function_collection functions;

	functions.add_scattering_function(std::make_shared<separable_bssrdf_reflection>(mMode, mEta));

	// modify the wo to shading_space.z
	// it will be used to find a wi in separable_bssrdf_reflection::sample() 
	interactions[which].wo = interactions[which].shading_space.z();
	
	return scattering_surface_sample(
		functions,
		interactions[which],
		evaluate_reflectance_profile(distance),
		pdf(interactions[which]) / interactions.size()
	);
}

rainbow::core::real rainbow::cpus::scatterings::separable_bidirectional_scattering_surface_distribution_function::pdf(
	const surface_interaction& interaction)
{
	const auto d = mInteraction.point - interaction.point;
	const auto local_d = world_to_local(mCoordinateSystem, d);
	const auto local_n = world_to_local(mCoordinateSystem, interaction.normal);

	const real projection[] = {
		math::sqrt(local_d.y * local_d.y + local_d.z * local_d.z),
		math::sqrt(local_d.z * local_d.z + local_d.x * local_d.x),
		math::sqrt(local_d.x * local_d.x + local_d.y * local_d.y)
	};

	const real channel_pdf = static_cast<real>(1) / spectrum::num_samples;
	const real axis_pdf[] = { 0.25, 0.25, 0.50 };

	real pdf = 0;

	for (size_t channel = 0; channel < spectrum::num_samples; channel++) {
		for (auto axis = 0; axis < 3; axis++) {
			pdf = pdf + pdf_reflectance_profile(channel, projection[axis])
				* math::abs(local_n[axis]) * channel_pdf * axis_pdf[axis];
		}
	}

	return pdf;
}
