#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/random_generator.hpp"
#include "../shared/utilities.hpp"
#include "../shared/math/math.hpp"

#include <vector>
#include <memory>

namespace rainbow {

	namespace samplers {

		template <size_t Dimension>
		class sampler_t : public interfaces::noncopyable {
		public:
			using sample_type = math::vector_t<Dimension, real>;
		public:
			explicit sampler_t(uint64 samples);
			
			explicit sampler_t(uint64 samples, uint64 seed);

			sample_type sample(const size_t index) const;

			size_t count() const noexcept;

			virtual std::shared_ptr<sampler_t<Dimension>> clone(uint64 seed) const;
			
			virtual sample_type next_sample();

			virtual void reset();
		protected:
			std::vector<sample_type> mSamples;

			random_generator mRandomGenerator;
			
			size_t mCurrentSampleIndex;
		};

		using sampler1d = sampler_t<1>;
		using sampler2d = sampler_t<2>;
		using sampler3d = sampler_t<3>;
		using sampler4d = sampler_t<4>;
	}
}

#include "detail/samplers.hpp"
