#pragma once

#include "math/math.hpp"

#include <array>

namespace rainbow {

	using namespace math;
	
	struct coordinate_system {
		std::array<vector3, 3> axes;

		coordinate_system();

		coordinate_system(const vector3& normal);
		
		coordinate_system(const std::array<vector3, 3>& axes);
		
		coordinate_system(const vector3& x, const vector3& y, const vector3& z);
		
		vector3 x() const noexcept;

		vector3 y() const noexcept;

		vector3 z() const noexcept;
	};

	vector3 world_to_local(const coordinate_system& system, const vector3& world_vector);

	vector3 local_to_world(const coordinate_system& system, const vector3& local_vector);
}
