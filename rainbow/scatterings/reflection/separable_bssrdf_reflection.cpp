#include "separable_bssrdf_reflection.hpp"

#include "../fresnel_effect.hpp"

namespace rainbow::scatterings {

    real fresnel_moment1(real eta) {
        const auto eta2 = eta * eta;
        const auto eta3 = eta2 * eta;
        const auto eta4 = eta3 * eta;
    	const auto eta5 = eta4 * eta;

    	if (eta < 1)
            return 0.45966f - 1.73965f * eta + 3.37668f * eta2 - 3.904945f * eta3 +
            2.49277f * eta4 - 0.68441f * eta5;
        else
            return -4.61686f + 11.1136f * eta - 10.4646f * eta2 + 5.11455f * eta3 -
            1.27198f * eta4 + 0.12746f * eta5;
    }
	
}

rainbow::separable_bssrdf_reflection::separable_bssrdf_reflection(real eta) :
	reflection_function(scattering_type::diffuse, spectrum(1)), mEta(eta)
{
}

rainbow::spectrum rainbow::separable_bssrdf_reflection::evaluate(const vector3& wo, const vector3& wi) const
{
    const auto c = 1 - 2 * fresnel_moment1(1 / mEta);
	const auto value = (1 - fresnel_reflect_dielectric(cos_theta(wi), 1, mEta)) / (c * pi<real>());

    return value * mEta * mEta;
}
