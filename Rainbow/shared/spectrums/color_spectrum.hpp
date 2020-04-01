#pragma once

#include "coefficient_spectrum.hpp"

#include "../math/math.hpp"

namespace rainbow {

	using namespace math;
	
	namespace spectrums {

		struct color_spectrum : public coefficient_spectrum<3> {
		public:
			color_spectrum() = default;

			color_spectrum(const real value);

			color_spectrum(const std::array<real, 3>& color);

			color_spectrum(const vector3& color);

			real red() const noexcept;

			real green() const noexcept;

			real blue() const noexcept;
		};
		
	}
}
