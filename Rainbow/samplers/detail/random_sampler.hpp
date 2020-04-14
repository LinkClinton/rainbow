#pragma once

#include "../random_sampler.hpp"

namespace rainbow {

	namespace samplers {

		template <size_t Dimension>
		random_sampler_t<Dimension>::random_sampler_t(const uint64 samples) : sampler_t<Dimension>(samples)
		{
		}

		template <size_t Dimension>
		std::shared_ptr<sampler_t<Dimension>> random_sampler_t<Dimension>::clone() const
		{
			return std::make_shared<random_sampler_t<Dimension>>(static_cast<uint64>(this->mSamples.size()));
		}

		template <size_t Dimension>
		typename sampler_t<Dimension>::sample_type random_sampler_t<Dimension>::next_sample()
		{
			if (this->mCurrentSampleIndex < this->mSamples.size())
				return this->mSamples[this->mCurrentSampleIndex++];

			++this->mCurrentSampleIndex;

			typename sampler_t<Dimension>::sample_type sample;

			for (auto dimension = 0; dimension < Dimension; dimension++) {
				sample[dimension] = this->mRandomGenerator.normalize_real();
			}

			return sample;
		}

		template <size_t Dimension>
		void random_sampler_t<Dimension>::reset()
		{
			for (size_t index = 0; index < this->mSamples.size(); index++) {
				auto& sample = this->mSamples[index];
				
				for (auto dimension = 0; dimension < Dimension; dimension++) {
					sample[dimension] = this->mRandomGenerator.normalize_real();
				}
			}
			
			sampler_t<Dimension>::reset();
		}
	}
}
