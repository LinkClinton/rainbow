#pragma once

#include "samplers.hpp"

namespace rainbow {

	namespace samplers {

		template <size_t Dimension>
		class random_sampler_t : public sampler_t<Dimension> {
		public:
			explicit random_sampler_t(const uint64 samples);

			void reset() override;
		};

		using random_sampler1d = random_sampler_t<1>;
		using random_sampler2d = random_sampler_t<2>;
		using random_sampler3d = random_sampler_t<3>;
		using random_sampler4d = random_sampler_t<4>;
	}
}

#include "detail/random_sampler.hpp"