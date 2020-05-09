#include "fresnel_effect.hpp"

rainbow::real rainbow::scatterings::fresnel_reflect_dielectric(
	real cos_theta_i, real eta_i, real eta_o)
{
	cos_theta_i = clamp(cos_theta_i, static_cast<real>(-1), static_cast<real>(1));

	// when cos_theta_i is less than 0, the input vector and normal are not the same hemisphere
	// so we need swap the eta_i and eta_o
	if (cos_theta_i < 0.f) {
		std::swap(eta_i, eta_o);

		cos_theta_i = abs(cos_theta_i);
	}

	const auto sin_theta_i = sqrt(max(static_cast<real>(0), 1 - cos_theta_i * cos_theta_i));
	const auto sin_theta_o = sin_theta_i * eta_i / eta_o;
	const auto cos_theta_o = sqrt(max(static_cast<real>(0), 1 - sin_theta_o * sin_theta_o));

	if (sin_theta_o >= 1) return 1;

	const auto r0 =
		(eta_o * cos_theta_i - eta_i * cos_theta_o) /
		(eta_o * cos_theta_i + eta_i * cos_theta_o);

	const auto r1 =
		(eta_i * cos_theta_i - eta_o * cos_theta_o) /
		(eta_i * cos_theta_i + eta_o * cos_theta_o);

	return (r0 * r0 + r1 * r1) / 2;
}

rainbow::spectrum rainbow::scatterings::fresnel_reflect_conductor(
	real cos_theta_i,
	const spectrum& eta_i, const spectrum& eta_o, const spectrum& k)
{
	cos_theta_i = clamp(cos_theta_i, static_cast<real>(-1), static_cast<real>(1));

	const spectrum eta = eta_o / eta_i;
	const spectrum eta_k = k / eta_i;

	const auto cos_theta_i_pow2 = cos_theta_i * cos_theta_i;
	const auto sin_theta_i_pow2 = 1 - cos_theta_i_pow2;

	const spectrum eta_pow2 = eta * eta;
	const spectrum eta_k_pow2 = eta_k * eta_k;

	const spectrum t0 = eta_pow2 - eta_k_pow2 - sin_theta_i_pow2;
	const spectrum a_pow2_plus_b_pow2 = sqrt(t0 * t0 + eta_pow2 * eta_k_pow2 * 4);
	const spectrum t1 = a_pow2_plus_b_pow2 + cos_theta_i_pow2;
	const spectrum a = sqrt((a_pow2_plus_b_pow2 + t0) * 0.5);
	const spectrum t2 = a * 2 * cos_theta_i;
	const spectrum t3 = a_pow2_plus_b_pow2 * cos_theta_i_pow2 + sin_theta_i_pow2 * sin_theta_i_pow2;
	const spectrum t4 = t2 * sin_theta_i_pow2;

	const spectrum r0 = (t1 - t2) / (t1 + t2);
	const spectrum r1 = r0 * (t3 - t4) / (t3 + t4);

	return (r0 + r1) * 0.5;
}

rainbow::scatterings::fresnel_effect_dielectric::fresnel_effect_dielectric(real eta_i, real eta_o) :
	mEtaI(eta_i), mEtaO(eta_o)
{

}

rainbow::spectrum rainbow::scatterings::fresnel_effect_dielectric::evaluate(real cos_theta_i) const
{
	return fresnel_reflect_dielectric(cos_theta_i, mEtaI, mEtaO);
}

rainbow::scatterings::fresnel_effect_conductor::fresnel_effect_conductor(const spectrum& eta_i, const spectrum& eta_o,
	const spectrum& k) : mEtaI(eta_i), mEtaO(eta_o), mK(k)
{

}

rainbow::spectrum rainbow::scatterings::fresnel_effect_conductor::evaluate(real cos_theta_i) const
{
	return fresnel_reflect_conductor(cos_theta_i, mEtaI, mEtaO, mK);
}

rainbow::spectrum rainbow::scatterings::fresnel_effect_nop::evaluate(real cos_theta_i) const
{
	return spectrum(1);
}
