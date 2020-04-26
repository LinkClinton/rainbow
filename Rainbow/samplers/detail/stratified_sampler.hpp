#pragma once

#include "../stratified_sampler.hpp"
#include "../../shared/logs/log.hpp"

namespace rainbow {

	namespace samplers {

		template <size_t Dimension>
		stratified_sampler_t<Dimension>::stratified_sampler_t(size_t samples_per_pixel_x, size_t samples_per_pixel_y, size_t dimension) :
			sampler_t<Dimension>(samples_per_pixel_x * samples_per_pixel_y), mSamples(dimension), mCurrentDimension(0),
			mSamplesPerPixelX(samples_per_pixel_x), mSamplesPerPixelY(samples_per_pixel_y)
		{
			for (size_t index = 0; index < mSamples.size(); index++)
				mSamples[index] = samples(this->mSamplesPerPixel);
		}

		template <size_t Dimension>
		stratified_sampler_t<Dimension>::stratified_sampler_t(size_t samples_per_pixel_x, size_t samples_per_pixel_y, size_t dimension, size_t seed) :
			sampler_t<Dimension>(samples_per_pixel_x * samples_per_pixel_y, seed), mSamples(dimension), mCurrentDimension(0),
			mSamplesPerPixelX(samples_per_pixel_x), mSamplesPerPixelY(samples_per_pixel_y)
		{
			for (size_t index = 0; index < mSamples.size(); index++)
				mSamples[index] = samples(this->mSamplesPerPixel);
		}

		template <size_t Dimension>
		stratified_sampler_t<Dimension>::stratified_sampler_t(size_t samples_per_pixel_x, size_t samples_per_pixel_y,
			size_t dimension, const std::shared_ptr<random_generator>& generator) :
			sampler_t<Dimension>(samples_per_pixel_x* samples_per_pixel_y, generator), mSamples(dimension), mCurrentDimension(0),
			mSamplesPerPixelX(samples_per_pixel_x), mSamplesPerPixelY(samples_per_pixel_y)
		{
			for (size_t index = 0; index < mSamples.size(); index++)
				mSamples[index] = samples(this->mSamplesPerPixel);
		}

		template <size_t Dimension>
		std::shared_ptr<sampler_t<Dimension>> stratified_sampler_t<Dimension>::clone(size_t seed) const
		{
			return std::make_shared<stratified_sampler_t<Dimension>>(mSamplesPerPixelX, mSamplesPerPixelY, mSamples.size(), seed);
		}

		template <size_t Dimension>
		std::shared_ptr<sampler_t<Dimension>> stratified_sampler_t<Dimension>::clone(
			const std::shared_ptr<random_generator>& generator) const
		{
			return std::make_shared<stratified_sampler_t<Dimension>>(mSamplesPerPixelX, mSamplesPerPixelY, mSamples.size(), generator);
		}

		template <size_t Dimension>
		typename sampler_t<Dimension>::sample_type stratified_sampler_t<Dimension>::next()
		{
			if (mCurrentDimension < mSamples.size())
				return mSamples[mCurrentDimension++][this->mCurrentSampleIndex];

			logs::warn("next sample will be random sample.");
			
			typename sampler_t<Dimension>::sample_type sample;

			for (auto index = 0; index < Dimension; index++)
				sample[index] = this->mRandomGenerator->uniform_real();

			return sample;
		}

		template <size_t Dimension>
		void stratified_sampler_t<Dimension>::next_sample()
		{
			mCurrentDimension = 0;

			sampler_t<Dimension>::next_sample();
		}

		template <size_t Dimension>
		void stratified_sampler_t<Dimension>::reset()
		{
			for (size_t index = 0; index < mSamples.size(); index++) {
				stratified_sample(mSamples[index]);

				shuffle(mSamples[index]);
			}
			
			sampler_t<Dimension>::reset();
		}

		template <>
		inline void stratified_sampler_t<1>::stratified_sample(samples& samples)
		{
			const auto inv_samples = static_cast<real>(1) / samples.size();
			const auto one = 1 - std::numeric_limits<real>::epsilon();

			for (size_t index = 0; index < samples.size(); index++) {
				const auto delta = mRandomGenerator->uniform_real();

				samples[index].x = math::min((index + delta) * inv_samples, one);
			}
		}

		template <size_t Dimension>
		void stratified_sampler_t<Dimension>::shuffle(samples& samples)
		{
			for (size_t index = 0; index < samples.size(); index++) {
				const auto other = index + this->mRandomGenerator->uint32(0, static_cast<uint32>(samples.size() - index - 1));

				std::swap(samples[index], samples[other]);
			}
		}

		template <>
		inline void stratified_sampler_t<2>::stratified_sample(samples& samples)
		{
			const auto dx = static_cast<real>(1) / mSamplesPerPixelX;
			const auto dy = static_cast<real>(1) / mSamplesPerPixelY;
			const auto one = 1 - std::numeric_limits<real>::epsilon();

			size_t index = 0;
			
			for (size_t y = 0; y < mSamplesPerPixelY; y++) {
				for (size_t x = 0; x < mSamplesPerPixelX; x++) {
					const auto delta_x = mRandomGenerator->uniform_real();
					const auto delta_y = mRandomGenerator->uniform_real();

					samples[index].x = math::min((x + delta_x) * dx, one);
					samples[index].y = math::min((y + delta_y) * dy, one);

					index++;
				}
			}
		}

	}
}
