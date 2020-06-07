#pragma once

#include "../distribution/microfacet_distribution.hpp"
#include "../fresnel_effect.hpp"

#include "reflection_function.hpp"

namespace rainbow::cpus::scatterings {

	class microfacet_reflection final : public reflection_function {
	public:
		explicit microfacet_reflection(
			const std::shared_ptr<microfacet_distribution>& distribution,
			const std::shared_ptr<fresnel_effect>& fresnel,
			const spectrum& reflectance);

		spectrum evaluate(const vector3& wo, const vector3& wi) const override;

		scattering_sample sample(const vector3& wo, const vector2& sample) const override;

		real pdf(const vector3& wo, const vector3& wi) const override;
	private:
		std::shared_ptr<microfacet_distribution> mDistribution;
		std::shared_ptr<fresnel_effect> mFresnel;
	};

}