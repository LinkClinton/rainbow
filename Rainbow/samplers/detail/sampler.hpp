#pragma once

#include "sampler.hpp"

namespace rainbow {

	namespace sampler {

		template <size_t Dimension>
		sampler_t<Dimension>::sampler_t(const uint64 samples) : mSamples(samples), mCurrentSampleIndex(0)
		{
		}

		template <size_t Dimension>
		typename sampler_t<Dimension>::sample_type sampler_t<Dimension>::sample(const size_t index) const
		{
			return mSamples[index];
		}

		template <size_t Dimension>
		typename sampler_t<Dimension>::sample_type sampler_t<Dimension>::next_sample()
		{
			return mSamples[mCurrentSampleIndex++];
		}

		template <size_t Dimension>
		void sampler_t<Dimension>::reset()
		{
			mCurrentSampleIndex = 0;
		}

	}
}
