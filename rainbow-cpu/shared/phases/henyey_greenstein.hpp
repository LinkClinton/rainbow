#pragma once

#include "phase_function.hpp"

namespace rainbow::cpus::shared::phases {

	class henyey_greenstein final : public phase_function {
	public:
		explicit henyey_greenstein(real g);

		real evaluate(const vector3& wo, const vector3& wi) const override;

		phase_sample sample(const interaction& interaction, const vector2& sample) const override;
	private:
		real mG;
	};
	
}
