#include "transform.hpp"

rainbow::transform::transform() : transform(math::identity<real>(), math::identity<real>())
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

bool rainbow::transform::operator==(const transform& right) const
{
	return mTransform == right.mTransform && mInverseTransform == right.mInverseTransform;
}

bool rainbow::transform::operator!=(const transform& right) const
{
	return !operator==(right);
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
		interaction.uv,
		interaction.index
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

rainbow::bound3 rainbow::transform::operator()(const bound3& bound) const
{
	auto result = bound3(
		transform_point(*this, bound.min),
		transform_point(*this, bound.max));

	result.union_it(transform_point(*this, vector3(bound.min.x, bound.min.y, bound.max.z)));
	result.union_it(transform_point(*this, vector3(bound.min.x, bound.max.y, bound.min.z)));
	result.union_it(transform_point(*this, vector3(bound.min.x, bound.max.y, bound.max.z)));
	result.union_it(transform_point(*this, vector3(bound.max.x, bound.min.y, bound.min.z)));
	result.union_it(transform_point(*this, vector3(bound.max.x, bound.min.y, bound.max.z)));
	result.union_it(transform_point(*this, vector3(bound.max.x, bound.max.y, bound.min.z)));

	return result;
}

rainbow::ray rainbow::transform::operator()(const ray& ray) const
{
	// if the transform has scale transform, the direction of will scale too
	// so the length is not right, we should scale it with length of direction
	// and normalize the direction again
	const auto direction = transform_vector(*this, ray.direction);
	
	return rainbow::ray(
		normalize(direction),
		transform_point(*this, ray.origin), 
		ray.length * length(direction));
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

rainbow::transform rainbow::transform::identity()
{
	return transform();
}

rainbow::transform rainbow::translate(const vector3& vec)
{
	return transform(math::translate(vec), math::translate(-vec));
}

rainbow::transform rainbow::rotate(real angle, const vector3& axis)
{
	const auto matrix = math::rotate(angle, axis);

	return transform(matrix, math::inverse(matrix));
}

rainbow::transform rainbow::scale(const vector3& vec)
{
	return transform(math::scale(vec), math::scale(static_cast<real>(1) / vec));
}

rainbow::transform rainbow::perspective(real fov, real width, real height, real near, real far)
{
	const auto matrix = math::perspective<real>(fov, width, height, near, far);

	return transform(matrix, inverse(matrix));
}

rainbow::transform rainbow::look_at(const vector3& origin, const vector3& target, const vector3& up)
{
	const auto matrix = math::look_at(origin, target, up);

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
	// before normalize it, we should convert it from vector4 to vector3
	return normalize(vector3(transpose(transform.inverse_matrix()) * vector4(normal, 0)));
}

rainbow::interaction rainbow::transform_interaction(const transform& transform, const interaction& interaction)
{
	return rainbow::interaction(
		transform_normal(transform, interaction.normal),
		transform_point(transform, interaction.point),
		transform_vector(transform, interaction.wo)
	);
}
