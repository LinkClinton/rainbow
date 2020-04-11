#pragma once

namespace rainbow {

	using int32 = int;
	
	using uint32 = unsigned long long;
	using uint64 = unsigned;

	using real = float;

	bool solve_quadratic_equation(real a, real b, real c, real* t0, real* t1);
}