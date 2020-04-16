#pragma once

#include "samplers.hpp"

namespace rainbow {

	namespace samplers {

		template <size_t Dimension>
		sampler_t<Dimension>::sampler_t(uint64 samples) : mSamples(samples), mCurrentSampleIndex(0)
		{
		}
		
		template <size_t Dimension>
		sampler_t<Dimension>::sampler_t(uint64 samples, uint64 seed) : mSamples(samples), mRandomGenerator(seed),
			mCurrentSampleIndex(0)
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
		std::shared_ptr<sampler_t<Dimension>> sampler_t<Dimension>::clone(uint64 seed) const {
			return std::make_shared<sampler_t<Dimension>>(static_cast<uint64>(mSamples.size()), seed);
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
