#include "color_spectrum.hpp"

#include "cie_samples.hpp"

namespace rainbow::cpus::shared::spectrums {

	real lerp(const real& s, const real& t, real x)
	{
		return s * (1 - x) + t * x;
	}

	real interpolate_spectrum_samples(
		const std::vector<real>& lambda,
		const std::vector<real>& value,
		real x)
	{
		if (x <= lambda.front()) return value.front();
		if (x >= lambda.back()) return value.back();

		const auto offset = static_cast<size_t>(std::lower_bound(lambda.begin(), lambda.end(), x) - lambda.begin());
		const auto t = (x - lambda[offset - 1]) / (lambda[offset] - lambda[offset - 1]);
		
		return lerp(value[offset - 1], value[offset], t);
	}
	
}

rainbow::cpus::shared::spectrums::color_spectrum::color_spectrum(const real value) : coefficient_spectrum<3>(value)
{
}

rainbow::cpus::shared::spectrums::color_spectrum::color_spectrum(const coefficient_spectrum<3>& coefficient_spectrum) :
	color_spectrum(coefficient_spectrum.coefficient)
{
}

rainbow::cpus::shared::spectrums::color_spectrum::color_spectrum(const std::vector<real>& lambda, const std::vector<real>& value)
{
	real x = 0, y = 0, z = 0;

	for (size_t index = 0; index < cie_samples; index++) {
		const auto sampled_value = interpolate_spectrum_samples(lambda, value, cie_samples_lambda[index]);

		x = x + sampled_value * cie_samples_x[index];
		y = y + sampled_value * cie_samples_y[index];
		z = z + sampled_value * cie_samples_z[index];
	}

	const auto scale = (cie_samples_lambda[cie_samples - 1] - cie_samples_lambda[0]) / (cie_samples_y_integral * cie_samples);

	x = x * scale;
	y = y * scale;
	z = z * scale;

	*this = from_xyz(x, y, z);
}

rainbow::cpus::shared::spectrums::color_spectrum::color_spectrum(const std::array<real, 3>& color) : coefficient_spectrum<3>(color)
{
}

rainbow::cpus::shared::spectrums::color_spectrum::color_spectrum(const vector3& color) :
	coefficient_spectrum<3>({ color.x, color.y, color.z })
{
}

rainbow::cpus::shared::spectrums::color_spectrum::color_spectrum(real red, real green, real blue) :
	color_spectrum(std::array<real, 3>{ red, green, blue })
{
}

rainbow::core::real rainbow::cpus::shared::spectrums::color_spectrum::red() const noexcept
{
	return coefficient[0];
}

rainbow::core::real rainbow::cpus::shared::spectrums::color_spectrum::green() const noexcept
{
	return coefficient[1];
}

rainbow::core::real rainbow::cpus::shared::spectrums::color_spectrum::blue() const noexcept
{
	return coefficient[2];
}

rainbow::cpus::shared::spectrums::color_spectrum rainbow::cpus::shared::spectrums::color_spectrum::from_xyz(real x, real y, real z)
{
	color_spectrum spectrum;
	
	spectrum.coefficient[0] = +3.240479f * x - 1.537150f * y - 0.498535f * z;
	spectrum.coefficient[1] = -0.969256f * x + 1.875991f * y + 0.041556f * z;
	spectrum.coefficient[2] = +0.055648f * x - 0.204043f * y + 1.057311f * z;

	return spectrum;
}

rainbow::core::real rainbow::cpus::shared::spectrums::color_spectrum::luminance() const noexcept
{
	return coefficient[0] * 0.212671f + coefficient[1] * 0.715160f + coefficient[2] * 0.072169f;
}
