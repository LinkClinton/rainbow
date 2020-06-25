#pragma once

#include "reflection_function.hpp"

#include "../scattering_surface_function.hpp"

namespace rainbow::cpus::scatterings {

	class separable_bssrdf_reflection final : public reflection_function {
	public:
		explicit separable_bssrdf_reflection(const transport_mode& mode, real eta);

		~separable_bssrdf_reflection() = default;

		spectrum evaluate(const vector3& wo, const vector3& wi) const override;
	private:
		transport_mode mMode;
		
		real mEta;
	};

}
