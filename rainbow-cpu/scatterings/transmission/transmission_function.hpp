#pragma once

#include "../scattering_function.hpp"

namespace rainbow::cpus::scatterings {

	class transmission_function : public scattering_function {
	public:
		explicit transmission_function(const scattering_type& type, const transport_mode& mode, const spectrum& transmission);

		~transmission_function() = default;

		scattering_sample sample(const vector3& wo, const vector2& sample) const override;

		real pdf(const vector3& wo, const vector3& wi) const override;
	protected:
		spectrum mTransmission;

		transport_mode mMode;
	};

}