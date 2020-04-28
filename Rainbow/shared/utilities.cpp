#include "utilities.hpp"

#include "math/math.hpp"

#include <algorithm>

bool rainbow::solve_quadratic_equation(real a, real b, real c, real* t0, real* t1)
{
	const auto discrim =
		static_cast<double>(b) *
		static_cast<double>(b) -
		static_cast<double>(a) *
		static_cast<double>(c) * 4;

	if (discrim < 0) return false;

	const auto root_discrim = sqrt(discrim);

	double q;
	if (b < 0)
		q = -0.5 * (b - root_discrim);
	else
		q = -0.5 * (b + root_discrim);
	
	*t0 = static_cast<real>(q / a);
	*t1 = static_cast<real>(c / q);
	
	if (*t0 > * t1) std::swap(*t0, *t1);

	return true;
}

rainbow::real rainbow::gamma_correct(real value)
{
	if (value <= 0.0031308f) return 12.92f * value;

	return 1.055f * pow(value, 1.f / 2.4f) - 0.055f;
}

rainbow::real rainbow::inverse_gamma_correct(real value)
{
	if (value <= 0.04045f) return value * 1.f / 12.92f;
	return pow((value + 0.055f) * 1.f / 1.055f, static_cast<real>(2.4f));
}

