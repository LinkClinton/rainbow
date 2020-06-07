#pragma once

#include "reflection_function.hpp"

#include "../fresnel_effect.hpp"

namespace rainbow::cpus::scatterings {

	class lambertian_reflection final : public reflection_function {
	public:
		explicit lambertian_reflection(const spectrum& reflectance);

		~lambertian_reflection() = default;

		spectrum evaluate(const vector3& wo, const vector3& wi) const override;
	};

}