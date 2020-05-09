#pragma once

#include "../random_sampler.hpp"

namespace rainbow {

	namespace samplers {

		template <size_t Dimension>
		random_sampler_t<Dimension>::random_sampler_t(size_t samples_per_pixel) :
			sampler_t<Dimension>(samples_per_pixel)
		{
		}

		template <size_t Dimension>
		random_sampler_t<Dimension>::random_sampler_t(size_t samples_per_pixel, size_t seed) :
			sampler_t<Dimension>(samples_per_pixel, seed)
		{
		}

		template <size_t Dimension>
		random_sampler_t<Dimension>::random_sampler_t(size_t samples_per_pixel,
			const std::shared_ptr<random_generator>& generator) :
			sampler_t<Dimension>(samples_per_pixel, generator)
		{
		}

		template <size_t Dimension>
		std::shared_ptr<sampler_t<Dimension>> random_sampler_t<Dimension>::clone(size_t seed) const
		{
			return std::make_shared<random_sampler_t<Dimension>>(this->mSamplesPerPixel, seed);
		}

		template <size_t Dimension>
		std::shared_ptr<sampler_t<Dimension>> random_sampler_t<Dimension>::clone(
			const std::shared_ptr<random_generator>& generator) const
		{
			return std::make_shared<random_sampler_t<Dimension>>(this->mSamplesPerPixel, generator);
		}

		template <size_t Dimension>
		typename sampler_t<Dimension>::sample_type random_sampler_t<Dimension>::next()
		{
			typename sampler_t<Dimension>::sample_type sample;

			for (auto index = 0; index < Dimension; index++) 
				sample[index] = this->mRandomGenerator->uniform_real();

			return sample;
		}

	}
}
