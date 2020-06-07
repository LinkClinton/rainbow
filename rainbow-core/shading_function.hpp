#pragma once

#include "math/math.hpp"

namespace rainbow::core {

	using namespace math;
	
	/*
	 * reflection coordinate system(shading space) is a build with three vectors s, t, n.
	 * 
	 * the theta is the angle between z-axis and vector
	 * the phi is the angle between the projected vector in x-y plane and x-axis
	 */
	real cos_theta(const vector3& w);
	real sin_theta(const vector3& w);
	real tan_theta(const vector3& w);
	real cos_phi(const vector3& w);
	real sin_phi(const vector3& w);

	real cos_theta_pow2(const vector3& w);
	real sin_theta_pow2(const vector3& w);
	real tan_theta_pow2(const vector3& w);
	real cos_phi_pow2(const vector3& w);
	real sin_phi_pow2(const vector3& w);

	bool same_hemisphere(const vector3& v0, const vector3& v1);

	vector3 face_forward(const vector3& v, const vector3& forward);

	vector3 spherical_direction(real sin_theta, real cos_theta, real phi);

	real spherical_theta(const vector3& v);

	real spherical_phi(const vector3& v);

	/*
	 * reflection : theta_o = theta_i, phi_o = phi_i + pi
	 * refraction : phi_o = phi_i + pi, eta_i * sin_theta_i = eta_o * sin_theta_o
	 * where eta_i or eta_o means the index of refraction.
	 * how much more slowly light travels in a particular medium than in a vacuum
	 * eta_i means the ior in input space, eta_o means ior in output space
	 *
	 * eta = eta_i / eta_o
	 */

	vector3 reflect(const vector3& wi, const vector3& normal);

	vector3 refract(const vector3& wi, const vector3& normal, real eta);
	
}
