#pragma once

#include "samplers.hpp"

namespace rainbow {

	namespace samplers {

		template <size_t Dimension>
		class random_sampler_t : public sampler_t<Dimension> {
		public:
			explicit random_sampler_t(size_t samples_per_pixel);
			
			explicit random_sampler_t(size_t samples_per_pixel, size_t seed);

			explicit random_sampler_t(size_t samples_per_pixel, const std::shared_ptr<random_generator>& generator);

			std::shared_ptr<sampler_t<Dimension>> clone(size_t seed) const override;

			std::shared_ptr<sampler_t<Dimension>> clone(const std::shared_ptr<random_generator>& generator) const override;

			typename sampler_t<Dimension>::sample_type next() override;
		};

		using random_sampler1d = random_sampler_t<1>;
		using random_sampler2d = random_sampler_t<2>;
		using random_sampler3d = random_sampler_t<3>;
		using random_sampler4d = random_sampler_t<4>;
	}
}

#include "detail/random_sampler.hpp"