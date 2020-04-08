#include "scattering_function.hpp"

rainbow::real rainbow::scatterings::cos_theta(const vector3& w)
{
	// the theta is the angle between w and z-axis
	// and the w is a normalize vector
	// so cos_theta = (0, 0, 1) dot w = w.z
	return w.z;
}

rainbow::real rainbow::scatterings::sin_theta(const vector3& w)
{
	// sin^2 theta + cos^2 theta = 1
	// so sin_theta = sqrt(1 - cos_theta_pow2)
	return sqrt(sin_theta_pow2(w));
}

rainbow::real rainbow::scatterings::tan_theta(const vector3& w)
{
	return sin_theta(w) / cos_theta(w);
}

rainbow::real rainbow::scatterings::cos_phi(const vector3& w)
{
	// the phi is the angle between projected w in x-y plane and x-axis
	// and the w is a normalize vector
	// so cos_phi = w.x / length(w.x, w,y, 0) = w.x / sin_theta
	const auto sin = sin_theta(w);

	return sin == 0 ? 1 : clamp(w.x / sin, 
		static_cast<real>(-1), 
		static_cast<real>(1));
}

rainbow::real rainbow::scatterings::sin_phi(const vector3& w)
{
	// the phi is the angle between projected w in x-y plane and x-axis
	// and the w is a normalize vector
	// so cos_phi = w.y / length(w.x, w,y, 0) = w.y / sin_theta
	const auto sin = sin_theta(w);

	return sin == 0 ? 1 : clamp(w.y / sin,
		static_cast<real>(-1),
		static_cast<real>(1));
}

rainbow::real rainbow::scatterings::cos_theta_pow2(const vector3& w)
{
	return cos_theta(w) * cos_theta(w);
}

rainbow::real rainbow::scatterings::sin_theta_pow2(const vector3& w)
{
	return max(static_cast<real>(0), 1 - cos_theta_pow2(w));
}

rainbow::real rainbow::scatterings::tan_theta_pow2(const vector3& w)
{
	return sin_theta_pow2(w) / cos_theta_pow2(w);
}

rainbow::real rainbow::scatterings::cos_phi_pow2(const vector3& w)
{
	return cos_phi(w) * cos_phi(w);
}

rainbow::real rainbow::scatterings::sin_phi_pow2(const vector3& w)
{
	return sin_phi(w) * sin_phi(w);
}

bool rainbow::scatterings::same_hemisphere(const vector3& v0, const vector3& v1)
{
	return v0.z * v1.z > 0;
}

rainbow::vector3 rainbow::scatterings::reflect(const vector3& wi, const vector3& normal)
{
	// because the origin of wi and wo is (0, 0, 0)
	// so the wi should be -wi. phi_o = phi_i + pi
	// normal * dot(wi, normal) is the length from the end point of wo to the x-y plane the normal on
	return -wi + 2 * dot(wi, normal) * normal;
}

rainbow::vector3 rainbow::scatterings::refract(const vector3& wi, const vector3& normal, real eta)
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

rainbow::scatterings::scattering_type rainbow::scatterings::operator|(const scattering_type& left,
	const scattering_type& right)
{
	return static_cast<scattering_type>(static_cast<uint32>(left) | static_cast<uint32>(right));
}

rainbow::scatterings::scattering_type rainbow::scatterings::operator&(const scattering_type& left,
	const scattering_type& right)
{
	return static_cast<scattering_type>(static_cast<uint32>(left) & static_cast<uint32>(right));
}

bool rainbow::scatterings::match(const scattering_type& target, const scattering_type& flag)
{
	return (target & flag) != scattering_type::unknown;
}

rainbow::scatterings::scattering_sample::scattering_sample(
	const scattering_type& type, 
	const spectrum& value,
	const vector3& wi, real pdf) :
	type(type), value(value), wi(wi), pdf(pdf)
{
}

rainbow::scatterings::bidirectional_scattering_distribution_function::bidirectional_scattering_distribution_function(
	const scattering_type& type) : mType(type)
{
}

rainbow::scatterings::scattering_type rainbow::scatterings::bidirectional_scattering_distribution_function::type() const noexcept
{
	return mType;
}
