#pragma once

#include "../../rainbow-core/shading_function.hpp"
#include "../../rainbow-core/math/math.hpp"

#include "../shared/spectrums/spectrum.hpp"

#include <vector>

namespace rainbow::cpus::scatterings {

	using namespace shared::spectrums;
	using namespace math;

	/*
	 * bidirectional_scattering_distribution_function is a function
	 * defined the scattering of two vector(wo, wi).
	 * wo is a normalize vector in reflection coordinate system means output vector
	 * wi is a normalize vector in reflection coordinate system means input vector
	 *
	 * bsdf::rho is a function that defined the integral of (wi with specific wo) or (wo and wi)
	 *
	 */

	enum class scattering_type : uint32 {
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

	bool has(const scattering_type& target, const scattering_type& flag);

	enum class transport_mode : uint32 {
		radiance = 0,
		important = 1
	};
	
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
