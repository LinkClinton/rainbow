#include "homogeneous_medium.hpp"

#include "../../rainbow-core/sample_function.hpp"

#include "../shared/phases/henyey_greenstein.hpp"

rainbow::cpus::media::homogeneous_medium::homogeneous_medium(const spectrum& sigma_a, const spectrum& sigma_s, real g) :
	mSigmaA(sigma_a), mSigmaS(sigma_s), mG(g), mSigmaT(sigma_a + sigma_s)
{
}

rainbow::cpus::shared::spectrums::spectrum rainbow::cpus::media::homogeneous_medium::evaluate(
	const std::shared_ptr<sampler1d>& sampler, const ray& ray)
{
	return exp(-mSigmaT * min(ray.length, std::numeric_limits<real>::max()));
}

rainbow::cpus::media::medium_sample rainbow::cpus::media::homogeneous_medium::sample(
	const std::shared_ptr<sampler1d>& sampler, const ray& ray)
{
	// sample the channel and find the t of sample point
	// if the t greater than ray.length, means we sample the surface at ray.end_point
	// otherwise, we sample the particle of medium at t-position
	// t = -ln(1 - sample) / sigma_t
	const auto channel = uniform_sample_one_from_range(0, spectrum::num_samples, sampler->next().x);
	const auto t = min(-log(1 - sampler->next().x) / mSigmaT[channel], ray.length);

	const auto sampled_medium = t < ray.length;

	// the pdf of sampled surface is e^(-sigma_t[i] * ray.length) / n
	// the pdf of sampled medium is sigma_t[i] * e^(-sigma_t[i] * t) / n
	// because t = min(ray.length, real_t),
	// e^(-sigma_t[i] * ray.length) = e^(-sigma_t[i] * t) = value of transmission beam from ray.origin to sampled point
	// density = value when we sampled surface, density = sigma_t * value when we sampled medium
	const auto value = exp(-mSigmaT * min(t, std::numeric_limits<real>::max()));
	const auto density = sampled_medium ? (mSigmaT * value) : value;

	real pdf = 0;

	for (size_t index = 0; index < spectrum::num_samples; index++)
		pdf = pdf + density[index];

	pdf = pdf / spectrum::num_samples;

	if (pdf == 0) pdf = 1;

	// the value of sampled surface is e^(-sigma_t[i] * ray.length) / pdf of sampled surface
	// the value of sampled medium is sigma_s * e^(-sigma_t[i] * t) / pdf of sampled medium
	if (!sampled_medium) return medium_sample(std::nullopt, value / pdf);

	return medium_sample(
		medium_interaction(
			std::make_shared<henyey_greenstein>(mG), 
			ray.origin + ray.direction * t, -ray.direction),
		value * mSigmaS / pdf
	);
}
