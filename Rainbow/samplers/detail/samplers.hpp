#pragma once

#include "samplers.hpp"

namespace rainbow {

	namespace samplers {

		template <size_t Dimension>
		sampler_t<Dimension>::sampler_t(size_t samples_per_pixel) :
			mCurrentSampleIndex(0), mSamplesPerPixel(samples_per_pixel)
		{
		}

		template <size_t Dimension>
		sampler_t<Dimension>::sampler_t(size_t samples_per_pixel, size_t seed) :
			mRandomGenerator(static_cast<uint64>(seed)), mCurrentSampleIndex(0),
			mSamplesPerPixel(samples_per_pixel)
		{
		}

		template <size_t Dimension>
		size_t sampler_t<Dimension>::samples_per_pixel() const noexcept
		{
			return mSamplesPerPixel;
		}

		template <size_t Dimension>
		void sampler_t<Dimension>::next_sample()
		{
			mCurrentSampleIndex++;
		}

		template <size_t Dimension>
		void sampler_t<Dimension>::reset()
		{
			mCurrentSampleIndex = 0;
		}

	}
}
