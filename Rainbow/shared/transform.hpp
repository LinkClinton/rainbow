#pragma once

#include "interactions/surface_interaction.hpp"
#include "math/math.hpp"

#include "ray.hpp"

namespace rainbow {

	using namespace interactions;
	using namespace math;
	
	class transform {
	public:
		transform();

		transform(const matrix4x4& transform, const matrix4x4& inverse);

		transform& operator*=(const transform& right);
		transform operator*(const transform& right) const;

		bool operator==(const transform& right) const;
		bool operator!=(const transform& right) const;
		
		surface_interaction operator()(const surface_interaction& interaction) const;
		
		coordinate_system operator()(const coordinate_system& system) const;

		bound3 operator()(const bound3& bound) const;
		
		ray operator()(const ray& ray) const;

		matrix4x4 inverse_matrix() const noexcept;

		matrix4x4 matrix() const noexcept;
		
		transform inverse() const;

		static transform identity();
	private:
		matrix4x4 mInverseTransform;
		matrix4x4 mTransform;
	};
	
	transform translate(const vector3& vec);

	transform rotate(const float angle, const vector3& axis);

	transform scale(const vector3& vec);

	transform perspective(const real fov, const real near = 0.01, const real far = 1000);

	vector3 transform_point(const transform& transform, const vector3& point);

	vector3 transform_vector(const transform& transform, const vector3& vector);

	vector3 transform_normal(const transform& transform, const vector3& normal);

	interaction transform_interaction(const transform& transform, const interaction& interaction);
}
