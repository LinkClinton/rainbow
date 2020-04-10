#pragma once

#include "samplers.hpp"

namespace rainbow {

	namespace samplers {

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
		size_t sampler_t<Dimension>::count() const noexcept
		{
			return mSamples.size();
		}
		
		template <size_t Dimension>
		typename sampler_t<Dimension>::sample_type sampler_t<Dimension>::next_sample()
		{
			assert(mCurrentSampleIndex < mSamples.size());
			
			return mSamples[mCurrentSampleIndex++];
		}

		template <size_t Dimension>
		void sampler_t<Dimension>::reset()
		{
			mCurrentSampleIndex = 0;
		}

	}
}
