#pragma once

#include "../shared/spectrums/spectrum.hpp"
#include "../shared/math/math.hpp"

#include <vector>

namespace rainbow {

	using namespace spectrums;
	using namespace math;
	
	namespace scatterings {

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
		 * bidirectional_scattering_distribution_function is a function
		 * defined the scattering of two vector(wo, wi).
		 * wo is a normalize vector in reflection coordinate system means output vector
		 * wi is a normalize vector in reflection coordinate system means input vector
		 *
		 * bsdf::rho is a function that defined the integral of (wi with specific wo) or (wo and wi)
		 * 
		 */

		enum class scattering_type : uint32{
			unknown,
			reflection = 1 << 0,
			transmission = 1 << 1,
			specular = 1 << 2,
			diffuse = 1 << 3,
			glossy = 1 << 4,
			all = reflection | transmission | specular | diffuse | glossy
		};

		scattering_type operator|(const scattering_type& left, const scattering_type& right);
		scattering_type operator&(const scattering_type& left, const scattering_type& right);
		scattering_type operator^(const scattering_type& left, const scattering_type& right);
		
		bool match(const scattering_type& target, const scattering_type& flag);

		struct scattering_sample {
			scattering_type type = scattering_type::unknown;
			spectrum value = spectrum(0);
			vector3 wi = vector3(0);
			real pdf = 0;

			scattering_sample() = default;

			scattering_sample(
				const scattering_type& type,
				const spectrum& value,
				const vector3& wi,
				real pdf);
		};
		
		class bidirectional_scattering_distribution_function {
		public:
			bidirectional_scattering_distribution_function() = default;

			explicit bidirectional_scattering_distribution_function(
				const scattering_type& type);
			
			virtual ~bidirectional_scattering_distribution_function() = default;

			virtual spectrum evaluate(const vector3& wo, const vector3& wi) const = 0;

			virtual scattering_sample sample(const vector3& wo, const vector2& sample) const = 0;
			
			virtual spectrum rho(const vector3& wo, const std::vector<vector2>& samples) const;

			virtual spectrum rho(const std::vector<vector2>& sample0, const std::vector<vector2>& sample1) const;

			virtual real pdf(const vector3& wo, const vector3& wi) const = 0;

			scattering_type type() const noexcept;
		protected:
			scattering_type mType = scattering_type::unknown;
		};

		using scattering_function = bidirectional_scattering_distribution_function;
	}
}
