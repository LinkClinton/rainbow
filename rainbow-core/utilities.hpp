#pragma once

namespace rainbow::core {

	using int32 = int;
	
	using uint32 = unsigned;
	using uint64 = unsigned long long;

	using real = float;

	bool solve_quadratic_equation(real a, real b, real c, real* t0, real* t1);

	real gamma_correct(real value);

	real inverse_gamma_correct(real value);

	real henyey_greenstein_phase_function(real cos_theta, real g);
}