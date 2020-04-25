#include "transform.hpp"

rainbow::transform::transform() : transform(identity<real>(), identity<real>())
{
}

rainbow::transform::transform(const matrix4x4& transform, const matrix4x4& inverse) :
	mInverseTransform(inverse), mTransform(transform)
{
}

rainbow::transform& rainbow::transform::operator*=(const transform& right)
{
	mTransform = mTransform * right.mTransform;
	mInverseTransform = right.mInverseTransform * mInverseTransform;

	return *this;
}

rainbow::transform rainbow::transform::operator*(const transform& right) const
{
	return transform(
		mTransform * right.mTransform,
		right.mInverseTransform * mInverseTransform
	);
}

rainbow::surface_interaction rainbow::transform::operator()(const surface_interaction& interaction) const
{
	return surface_interaction(
		interaction.entity,
		(*this)(interaction.shading_space),
		transform_vector(*this, interaction.dp_du),
		transform_vector(*this, interaction.dp_dv),
		transform_normal(*this, interaction.normal),
		transform_point(*this, interaction.point),
		normalize(transform_vector(*this, interaction.wo)),
		interaction.uv
	);
}

rainbow::coordinate_system rainbow::transform::operator()(const coordinate_system& system) const
{
	auto shading_space_x = transform_vector(*this, system.x());
	auto shading_space_y = transform_vector(*this, system.y());
	auto shading_space_z = normalize(math::cross(shading_space_x, shading_space_y));

	shading_space_x = normalize(shading_space_x);
	shading_space_y = normalize(math::cross(shading_space_z, shading_space_x));

	return coordinate_system(
		shading_space_x,
		shading_space_y,
		shading_space_z
	);
}

rainbow::ray rainbow::transform::operator()(const ray& ray) const
{
	return rainbow::ray(
		normalize(transform_vector(*this, ray.direction)),
		transform_point(*this, ray.origin), 
		ray.length);
}

rainbow::matrix4x4 rainbow::transform::inverse_matrix() const noexcept
{
	return mInverseTransform;
}

rainbow::matrix4x4 rainbow::transform::matrix() const noexcept
{
	return mTransform;
}

rainbow::transform rainbow::transform::inverse() const
{
	return transform(mInverseTransform, mTransform);
}

rainbow::transform rainbow::translate(const vector3& vec)
{
	return transform(math::translate(vec), math::translate(-vec));
}

rainbow::transform rainbow::rotate(const float angle, const vector3& axis)
{
	const auto matrix = math::rotate(angle, axis);

	return transform(matrix, math::inverse(matrix));
}

rainbow::transform rainbow::scale(const vector3& vec)
{
	return transform(math::scale(vec), math::scale(static_cast<real>(1) / vec));
}

rainbow::transform rainbow::perspective(const real fov, const real near, const real far)
{
	const auto matrix = math::perspective<real>(fov, near, far);
	
	return transform(matrix, inverse(matrix));
}

rainbow::vector3 rainbow::transform_point(const transform& transform, const vector3& point)
{
	const auto v = transform.matrix() * vector4(point, 1);

	if (v.w == 1) return v;

	return v / v.w;
}

rainbow::vector3 rainbow::transform_vector(const transform& transform, const vector3& vector)
{
	return transform.matrix() * vector4(vector, 0);
}

rainbow::vector3 rainbow::transform_normal(const transform& transform, const vector3& normal)
{
	return normalize(transpose(transform.inverse_matrix()) * vector4(normal, 0));
}

rainbow::interaction rainbow::transform_interaction(const transform& transform, const interaction& interaction)
{
	return rainbow::interaction(
		transform_normal(transform, interaction.normal),
		transform_point(transform, interaction.point),
		transform_vector(transform, interaction.wo)
	);
}
