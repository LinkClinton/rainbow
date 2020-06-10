#pragma once

#include "../distribution/microfacet_distribution.hpp"
#include "reflection_function.hpp"

#include <memory>

namespace rainbow::cpus::scatterings {

	class fresnel_blend_reflection final : public reflection_function {
	public:
		explicit fresnel_blend_reflection(
			const std::shared_ptr<microfacet_distribution>& distribution,
			const spectrum& reflectance_specular,
			const spectrum& reflectance_diffuse,
			const spectrum& scale = spectrum(1));

		spectrum evaluate(const vector3& wo, const vector3& wi) const override;

		scattering_sample sample(const vector3& wo, const vector2& sample) const override;

		real pdf(const vector3& wo, const vector3& wi) const override;
	private:
		spectrum schlick_fresnel(real cos_theta) const noexcept;

		real pow_5(real value) const noexcept;

		std::shared_ptr<microfacet_distribution> mDistribution;

		spectrum mReflectanceSpecular;
		spectrum mReflectanceDiffuse;
		spectrum mScale;
	};

}
