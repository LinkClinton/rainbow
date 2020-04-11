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
