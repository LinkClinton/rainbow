#pragma once

#include "math/math.hpp"

namespace rainbow {

	using namespace math;
	
	struct ray {
		vector3 direction;
		vector3 origin;

		mutable real length;

		ray();

		ray(const vector3& direction, const vector3& origin,
			const real length = std::numeric_limits<real>::infinity());

		vector3 begin_point() const;

		vector3 end_point() const;
	};
	
}
