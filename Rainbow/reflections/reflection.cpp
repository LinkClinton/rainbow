#include "reflection.hpp"

rainbow::real rainbow::reflections::cos_theta(const vector3& w)
{
	// the theta is the angle between w and z-axis
	// and the w is a normalize vector
	// so cos_theta = (0, 0, 1) dot w = w.z
	return w.z;
}

rainbow::real rainbow::reflections::sin_theta(const vector3& w)
{
	// sin^2 theta + cos^2 theta = 1
	// so sin_theta = sqrt(1 - cos_theta_pow2)
	return sqrt(sin_theta_pow2(w));
}

rainbow::real rainbow::reflections::tan_theta(const vector3& w)
{
	return sin_theta(w) / cos_theta(w);
}

rainbow::real rainbow::reflections::cos_phi(const vector3& w)
{
	// the phi is the angle between projected w in x-y plane and x-axis
	// and the w is a normalize vector
	// so cos_phi = w.x / length(w.x, w,y, 0) = w.x / sin_theta
	const auto sin = sin_theta(w);

	return sin == 0 ? 1 : clamp(w.x / sin, 
		static_cast<real>(-1), 
		static_cast<real>(1));
}

rainbow::real rainbow::reflections::sin_phi(const vector3& w)
{
	// the phi is the angle between projected w in x-y plane and x-axis
	// and the w is a normalize vector
	// so cos_phi = w.y / length(w.x, w,y, 0) = w.y / sin_theta
	const auto sin = sin_theta(w);

	return sin == 0 ? 1 : clamp(w.y / sin,
		static_cast<real>(-1),
		static_cast<real>(1));
}

rainbow::real rainbow::reflections::cos_theta_pow2(const vector3& w)
{
	return cos_theta(w) * cos_theta(w);
}

rainbow::real rainbow::reflections::sin_theta_pow2(const vector3& w)
{
	return max(static_cast<real>(0), 1 - cos_theta_pow2(w));
}

rainbow::real rainbow::reflections::tan_theta_pow2(const vector3& w)
{
	return sin_theta_pow2(w) / cos_theta_pow2(w);
}

rainbow::real rainbow::reflections::cos_phi_pow2(const vector3& w)
{
	return cos_phi(w) * cos_phi(w);
}

rainbow::real rainbow::reflections::sin_phi_pow2(const vector3& w)
{
	return sin_phi(w) * sin_phi(w);
}

bool rainbow::reflections::same_hemisphere(const vector3& v0, const vector3& v1)
{
	return v0.z * v1.z > 0;
}

rainbow::vector3 rainbow::reflections::reflect(const vector3& wi, const vector3& normal)
{
	// because the origin of wi and wo is (0, 0, 0)
	// so the wi should be -wi. phi_o = phi_i + pi
	// normal * dot(wi, normal) is the length from the end point of wo to the x-y plane the normal on
	return -wi + 2 * dot(wi, normal) * normal;
}

rainbow::vector3 rainbow::reflections::refract(const vector3& wi, const vector3& normal, real eta)
{
	// wi and normal are normalize vector
	const auto cos_theta_i = dot(wi, normal);
	const auto sin_theta_i_pow2 = max(static_cast<real>(0), 1 - cos_theta_i * cos_theta_i);

	// eta = eta_i / eta_o
	// eta_i * sin_theta_i = eta_o * sin_theta_o
	// sin_theta_o = sin_theta_i * eta_i / eta_o = sin_theta_i * eta
	// sin_theta_o_pow2 = sin_theta_i_pow2 * eta * eta
	const auto sin_theta_o_pow2 = sin_theta_i_pow2 * eta * eta;

	if (sin_theta_o_pow2 >= 1) return vector3(0);

	const auto cos_theta_o = sqrt(1 - sin_theta_o_pow2);

	return -wi * eta + (cos_theta_i * eta - cos_theta_o) * normal;
}

rainbow::real rainbow::reflections::fresnel_reflect_dielectric(
	real cos_theta_i, real eta_i, real eta_o)
{
	cos_theta_i = clamp(cos_theta_i, static_cast<real>(-1), static_cast<real>(1));

	// when cos_theta_i is less than 0, the input vector and normal are not the same hemisphere
	// so we need swap the eta_i and eta_o
	/*if (cos_theta_i < 0.f) {
		std::swap(eta_i, eta_o);

		cos_theta_i = abs(cos_theta_i);
	}*/

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

rainbow::spectrum rainbow::reflections::fresnel_reflect_conductor(
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
	const spectrum t3 = a_pow2_plus_b_pow2  * cos_theta_i_pow2 + sin_theta_i_pow2 * sin_theta_i_pow2;
	const spectrum t4 = t2 * sin_theta_i_pow2;

	const spectrum r0 = (t1 - t2) / (t1 + t2);
	const spectrum r1 = r0 * (t3 - t4) / (t3 + t4);

	return (r0 + r1) * 0.5;
}
