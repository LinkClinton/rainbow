#pragma once

#include "reflection_function.hpp"
#include "../fresnel_effect.hpp"

#include <memory>

namespace rainbow::cpus::scatterings {

	class specular_reflection final : public reflection_function {
	public:
		explicit specular_reflection(
			const std::shared_ptr<fresnel_effect>& fresnel,
			const spectrum& reflectance = 1);

		~specular_reflection() = default;

		spectrum evaluate(const vector3& wo, const vector3& wi) const override;

		scattering_sample sample(const vector3& wo, const vector2& sample) const override;

		real pdf(const vector3& wo, const vector3& wi) const override;
	private:
		std::shared_ptr<fresnel_effect> mFresnel;
	};

}