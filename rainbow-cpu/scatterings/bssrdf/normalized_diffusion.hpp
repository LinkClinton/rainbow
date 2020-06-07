#pragma once

#include "../scattering_surface_function.hpp"

namespace rainbow::cpus::scatterings {

	class normalized_diffusion final : public separable_bidirectional_scattering_surface_distribution_function {
	public:
		explicit normalized_diffusion(
			const surface_interaction& interaction,
			const spectrum& A, const spectrum& L,
			real eta);

		~normalized_diffusion() = default;
	protected:
		spectrum evaluate_reflectance_profile(real distance) override;

		real sample_reflectance_profile(size_t channel, real sample) override;

		real pdf_reflectance_profile(size_t channel, real distance) override;
	private:
		spectrum mD, mA, mL, mS;
	};

}
