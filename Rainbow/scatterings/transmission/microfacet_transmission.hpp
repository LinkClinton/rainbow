#pragma once

#include "../distribution/microfacet_distribution.hpp"
#include "../fresnel_effect.hpp"

#include "transmission_function.hpp"

namespace rainbow {

	namespace scatterings {

		class microfacet_transmission final : public transmission_function {
		public:
			explicit microfacet_transmission(
				const std::shared_ptr<microfacet_distribution>& distribution,
				const spectrum& transmission,
				real eta_i, real eta_o);

			~microfacet_transmission() = default;

			spectrum evaluate(const vector3& wo, const vector3& wi) const override;

			scattering_sample sample(const vector3& wo, const vector2& sample) const override;

			real pdf(const vector3& wo, const vector3& wi) const override;
		private:
			std::shared_ptr<microfacet_distribution> mDistribution;
			std::shared_ptr<fresnel_effect> mFresnel;

			real mEtaI;
			real mEtaO;
		};
		
	}
}
