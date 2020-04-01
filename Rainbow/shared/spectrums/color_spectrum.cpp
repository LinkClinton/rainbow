#include "color_spectrum.hpp"

rainbow::spectrums::color_spectrum::color_spectrum(const real value) : coefficient_spectrum<3>(value)
{
}

rainbow::spectrums::color_spectrum::color_spectrum(const std::array<real, 3>& color) : coefficient_spectrum<3>(color)
{
}

rainbow::spectrums::color_spectrum::color_spectrum(const vector3& color) :
	coefficient_spectrum<3>({ color.x, color.y, color.z })
{
}

rainbow::real rainbow::spectrums::color_spectrum::red() const noexcept
{
	return coefficient[0];
}

rainbow::real rainbow::spectrums::color_spectrum::green() const noexcept
{
	return coefficient[1];
}

rainbow::real rainbow::spectrums::color_spectrum::blue() const noexcept
{
	return coefficient[2];
}
