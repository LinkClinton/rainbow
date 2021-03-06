#include "environment_light.hpp"

#include "../../rainbow-core/shading_function.hpp"
#include "../../rainbow-core/sample_function.hpp"

#include "../textures/constant_texture.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::emitters::environment_light::environment_light(const spectrum& intensity, real radius) :
	environment_light(std::make_shared<constant_texture2d<spectrum>>(spectrum(1)), intensity, radius)
{
}

rainbow::cpus::emitters::environment_light::environment_light(
	const std::shared_ptr<texture2d<spectrum>>& environment_map,
	const spectrum& intensity, real radius) :
	emitter(emitter_type::environment), mEnvironmentMap(environment_map), mIntensity(intensity), mRadius(radius)
{
	const auto size = vector2_t<size_t>(mEnvironmentMap->size().x * 2, mEnvironmentMap->size().y * 2);

	auto distribution = std::vector<real>(size.x * size.y);

	for (size_t y = 0; y < size.y; y++) {
		const auto y_position = (y + static_cast<real>(0.5)) / size.y;
		const auto sin_theta = sin(y_position * pi<real>());

		for (size_t x = 0; x < size.x; x++) {
			const auto x_position = (x + static_cast<real>(0.5)) / size.x;
			const auto sample_point = vector2(x_position, y_position);
			
			distribution[x + y * size.x] = mEnvironmentMap->sample(sample_point).luminance() * sin_theta;
		}
	}

	mDistribution = std::make_shared<distribution2d>(distribution, size.x, size.y);
}

spectrum rainbow::cpus::emitters::environment_light::evaluate(
	const interaction& interaction, const vector3& wi) const
{
	// in environment light, the interaction is always with default interaction.
	// the wi will be the inverse direction of the trace ray
	const auto w = normalize(-wi);

	// convert the vector into spherical direction and mapped it to [0, 1]
	// we will sample the texture with (phi / two_pi, theta / pi)
	// do not remember add the intensity factor
	return mIntensity * mEnvironmentMap->sample(vector2(
		spherical_phi(w) * one_over_two_pi<real>(),
		spherical_theta(w) * one_over_pi<real>()
	));
}

rainbow::cpus::emitters::emitter_ray_sample rainbow::cpus::emitters::environment_light::sample(
	const shape_instance_properties& properties, const vector2& sample0, const vector2& sample1) const
{
	// first, sample the distribution to find the direction of ray
	const auto distribution_sample = mDistribution->sample(sample0);

	if (distribution_sample.pdf == 0) return {};

	const auto theta = distribution_sample.value.y * pi<real>();
	const auto phi = distribution_sample.value.x * two_pi<real>();

	const auto cos_theta = cos(theta);
	const auto sin_theta = sin(theta);

	// build the direction with spherical direction and inverse the direction
	const auto direction = -spherical_direction(sin_theta, cos_theta, phi);

	const auto system = coordinate_system(-direction);
	const auto point_in_disk = concentric_sample_disk(sample1);
	const auto point_in_local = mRadius * (point_in_disk.x * system.x() + point_in_disk.y * system.y());

	return emitter_ray_sample(
		mIntensity * mEnvironmentMap->sample(distribution_sample.value),
		direction,
		ray(direction, point_in_local + -direction * mRadius),
		sin_theta != 0 ? distribution_sample.pdf / (two_pi<real>() * pi<real>() * sin_theta) : 0,
		1 / (pi<real>() * mRadius * mRadius)
	);
}

rainbow::cpus::emitters::emitter_sample rainbow::cpus::emitters::environment_light::sample(
	const shape_instance_properties& properties, const interaction& reference, const vector2& sample) const
{
	const auto distribution_sample = mDistribution->sample(sample);

	if (distribution_sample.pdf == 0) return {};

	const auto theta = distribution_sample.value.y * pi<real>();
	const auto phi = distribution_sample.value.x * two_pi<real>();

	const auto cos_theta = cos(theta);
	const auto sin_theta = sin(theta);
	
	const auto pdf = sin_theta != 0 ? distribution_sample.pdf / (two_pi<real>() * pi<real>() * sin_theta) : 0;
	const auto wi = spherical_direction(sin_theta, cos_theta, phi);

	return emitter_sample(
		interaction(reference.point + 2 * mRadius * wi),
		mIntensity * mEnvironmentMap->sample(distribution_sample.value),
		wi,
		pdf
	);
}

std::tuple<real, real> rainbow::cpus::emitters::environment_light::pdf(
	const shape_instance_properties& properties, const ray& ray, const vector3& normal) const
{
	// the ray is spawned from the emitter, so we need inverse it direction to find theta and phi
	const auto direction = normalize(-ray.direction);

	const auto theta = spherical_theta(direction);
	const auto phi = spherical_phi(direction);

	const auto sin_theta = sin(theta);

	if (sin_theta == 0) return { static_cast<real>(0), static_cast<real>(0) };
	
	const auto distribution_pdf = mDistribution->pdf(vector2(
		phi * one_over_two_pi<real>(),
		theta * one_over_pi<real>()));

	// [pdf_position, pdf_direction]
	return { 1 / (pi<real>() * mRadius * mRadius), distribution_pdf / (two_pi<real>() * pi<real>() * sin_theta) };
}

rainbow::core::real rainbow::cpus::emitters::environment_light::pdf(
	const shape_instance_properties& properties, const interaction& reference, const vector3& wi) const
{
	// in environment light, the interaction is always with default interaction.
	// the wi will be the trace ray
	const auto w = normalize(wi);

	const auto theta = spherical_theta(w);
	const auto phi = spherical_phi(w);

	const auto sin_theta = sin(theta);

	if (sin_theta == 0) return 0;

	const auto distribution_pdf = mDistribution->pdf(vector2(
		phi * one_over_two_pi<real>(),
		theta * one_over_pi<real>()));
	
	return distribution_pdf / (two_pi<real>() * pi<real>() * sin_theta);
}

spectrum rainbow::cpus::emitters::environment_light::power(const shape_instance_properties& properties) const
{
	return mIntensity * mRadius * mRadius * pi<real>() * mEnvironmentMap->sample(vector2(0.5));
}

real rainbow::cpus::emitters::environment_light::radius() const noexcept
{
	return mRadius;
}
