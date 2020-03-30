#pragma once

#include "vector1.hpp"
#include "vector2.hpp"
#include "vector3.hpp"
#include "vector4.hpp"

#include "matrix4x4.hpp"

namespace rainbow {

	using real = float;

	using vector1 = vector1_t<real>;
	using vector2 = vector2_t<real>;
	using vector3 = vector3_t<real>;
	using vector4 = vector4_t<real>;

	using matrix4x4 = matrix4x4_t<real>;
}
