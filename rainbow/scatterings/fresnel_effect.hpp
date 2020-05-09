#pragma once

#include "scattering_function.hpp"

namespace rainbow {

	namespace scatterings {


		/*
		 * fresnel_reflect_xxx that help us to compute the ratio of reflection in scatting
		 * fresnel_reflect_dielectric is used for dielectric material
		 * fresnel_reflect_conductor is used for conductor material
		 *
		 * k is the absorption coefficient for conductor
		 */

		real fresnel_reflect_dielectric(real cos_theta_i, real eta_i, real eta_o);

		spectrum fresnel_reflect_conductor(real cos_theta_i,
			const spectrum& eta_i,
			const spectrum& eta_o,
			const spectrum& k);

		class fresnel_effect {
		public:
			fresnel_effect() = default;

			virtual ~fresnel_effect() = default;

			virtual spectrum evaluate(real cos_theta_i) const = 0;
		};

		class fresnel_effect_dielectric : public fresnel_effect {
		public:
			explicit fresnel_effect_dielectric(real eta_i, real eta_o);

			~fresnel_effect_dielectric() = default;

			spectrum evaluate(real cos_theta_i) const override;

		private:
			real mEtaI;
			real mEtaO;
		};

		class fresnel_effect_conductor : public fresnel_effect {
		public:
			explicit fresnel_effect_conductor(const spectrum& eta_i, const spectrum& eta_o, const spectrum& k);

			~fresnel_effect_conductor() = default;

			spectrum evaluate(real cos_theta_i) const override;

		private:
			spectrum mEtaI;
			spectrum mEtaO;
			spectrum mK;
		};

		class fresnel_effect_nop : public fresnel_effect {
		public:
			fresnel_effect_nop() = default;

			~fresnel_effect_nop() = default;

			spectrum evaluate(real cos_theta_i) const override;
		};
	}
}
