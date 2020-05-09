#pragma once

#include "coefficient_spectrum.hpp"

#include "../math/math.hpp"

#include <vector>

namespace rainbow {

	using namespace math;
	
	namespace spectrums {

		struct color_spectrum : public coefficient_spectrum<3> {
		public:
			color_spectrum() = default;

			color_spectrum(const real value);

			color_spectrum(const coefficient_spectrum<3>& coefficient_spectrum);

			color_spectrum(const std::vector<real>& lambda, const std::vector<real>& value);
			
			color_spectrum(const std::array<real, 3>& color);

			color_spectrum(const vector3& color);

			color_spectrum(real red, real green, real blue);
			
			real luminance() const noexcept;
			
			real red() const noexcept;

			real green() const noexcept;

			real blue() const noexcept;

			static color_spectrum from_xyz(real x, real y, real z);
		};
		
	}
}
