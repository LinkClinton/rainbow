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
			explicit sampler_t(size_t samples_per_pixel);
			
			explicit sampler_t(size_t samples_per_pixel, size_t seed);

			explicit sampler_t(size_t samples_per_pixel, const std::shared_ptr<random_generator>& generator);
			
			size_t samples_per_pixel() const noexcept;
			
			virtual std::shared_ptr<sampler_t<Dimension>> clone(size_t seed) const = 0;

			virtual std::shared_ptr<sampler_t<Dimension>> clone(const std::shared_ptr<random_generator>& generator) const = 0;
			
			virtual sample_type next() = 0;
			
			virtual void next_sample();

			virtual void reset();
		protected:
			std::shared_ptr<random_generator> mRandomGenerator;

			size_t mCurrentSampleIndex;
			size_t mSamplesPerPixel;
		};

		using sampler1d = sampler_t<1>;
		using sampler2d = sampler_t<2>;
		using sampler3d = sampler_t<3>;
		using sampler4d = sampler_t<4>;
	}
}

#include "detail/samplers.hpp"
