#pragma once

#include "math/math.hpp"

namespace rainbow {
	
	using namespace math;

	class transform {
	public:
		transform();

		transform(const matrix4x4& transform, const matrix4x4& inverse);

		transform& operator*=(const transform& right);
		transform operator*(const transform& right) const;

		vector4 operator()(const vector2& vec) const;
		vector4 operator()(const vector3& vec) const;
		vector4 operator()(const vector4& vec) const;

		transform inverse() const;
	private:
		matrix4x4 mInverseTransform;
		matrix4x4 mTransform;
	};

	transform translate(const vector3& vec);

	transform rotate(const float angle, const vector3& axis);

	transform scale(const vector3& vec);
}
