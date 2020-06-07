#pragma once

#include "../../rainbow-core/math/math.hpp"

namespace rainbow::cpus::shared {

	using namespace core::math;
	using namespace core;

	constexpr auto ray_epsilon = std::numeric_limits<real>::epsilon() / 2 * 1500;
	constexpr auto shadow_epsilon = ray_epsilon * 10;
	
	struct ray {
		vector3 direction;
		vector3 origin;

		mutable real length;

		ray();

		ray(const vector3& direction, const vector3& origin, real length = std::numeric_limits<real>::infinity());

		vector3 begin_point() const;

		vector3 end_point() const;
	};
	
}
