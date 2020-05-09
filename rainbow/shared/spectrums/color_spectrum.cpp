#include "color_spectrum.hpp"

rainbow::spectrums::color_spectrum::color_spectrum(const real value) : coefficient_spectrum<3>(value)
{
}

rainbow::spectrums::color_spectrum::color_spectrum(const coefficient_spectrum<3>& coefficient_spectrum) :
	color_spectrum(coefficient_spectrum.coefficient)
{
}

rainbow::spectrums::color_spectrum::color_spectrum(const std::array<real, 3>& color) : coefficient_spectrum<3>(color)
{
}

rainbow::spectrums::color_spectrum::color_spectrum(const vector3& color) :
	coefficient_spectrum<3>({ color.x, color.y, color.z })
{
}

rainbow::spectrums::color_spectrum::color_spectrum(real red, real green, real blue) :
	color_spectrum(std::array<real, 3>{ red, green, blue })
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

rainbow::real rainbow::spectrums::color_spectrum::luminance() const noexcept
{
	return coefficient[0] * 0.212671f + coefficient[1] * 0.715160f + coefficient[2] * 0.072169f;
}
