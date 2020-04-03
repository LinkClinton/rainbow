#pragma once

#include "../shared/spectrums/spectrum.hpp"
#include "../shared/math/math.hpp"

#include <vector>

namespace rainbow {

	using namespace spectrums;
	using namespace math;
	
	namespace reflections {

		/*
		 * reflection coordinate system is a build with three vectors s, t, n.
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
		
		/*
		 * bidirectional_scattering_distribution_function is a function
		 * defined the scattering of two vector(wo, wi).
		 * wo is a normalize vector in reflection coordinate system means output vector
		 * wi is a normalize vector in reflection coordinate system means input vector
		 *
		 * bsdf::rho is a function that defined the integral of (wi with specific wo) or (wo and wi)
		 * 
		 */
		
		class bidirectional_scattering_distribution_function {
		public:
			bidirectional_scattering_distribution_function() = default;

			virtual ~bidirectional_scattering_distribution_function() = default;

			virtual spectrum evaluate(const vector3& wo, const vector3& wi) const = 0;

			virtual spectrum rho(const vector3& wo, const std::vector<vector2>& samples) const = 0;

			virtual spectrum rho(const std::vector<vector2>& sample0, const std::vector<vector2>& sample1) const = 0;
		};
	}
}
