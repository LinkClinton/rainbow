#pragma once

#include "../scattering_function.hpp"

namespace rainbow::cpus::scatterings {

	class fresnel_specular final : public scattering_function {
	public:
		explicit fresnel_specular(
			const transport_mode& mode, const spectrum& transmission, 
			const spectrum& reflectance, real eta_i, real eta_o);

		spectrum evaluate(const vector3& wo, const vector3& wi) const override;

		scattering_sample sample(const vector3& wo, const vector2& sample) const override;

		real pdf(const vector3& wo, const vector3& wi) const override;
	private:
		spectrum mTransmission;
		spectrum mReflectance;

		transport_mode mMode;
		
		real mEtaI;
		real mEtaO;
	};

}
