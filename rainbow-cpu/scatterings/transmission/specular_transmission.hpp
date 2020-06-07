#pragma once

#include "../fresnel_effect.hpp"

#include "transmission_function.hpp"

namespace rainbow::cpus::scatterings {

	class specular_transmission final : public transmission_function {
	public:
		explicit specular_transmission(const spectrum& transmission, real eta_i, real eta_o);

		~specular_transmission() = default;

		spectrum evaluate(const vector3& wo, const vector3& wi) const override;

		scattering_sample sample(const vector3& wo, const vector2& sample) const override;

		real pdf(const vector3& wo, const vector3& wi) const override;
	private:
		std::shared_ptr<fresnel_effect> mFresnel;

		real mEtaI;
		real mEtaO;
	};

}
