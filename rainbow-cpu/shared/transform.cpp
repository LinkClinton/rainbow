#include "transform.hpp"

rainbow::cpus::shared::transform::transform() : transform(math::identity<real>(), math::identity<real>())
{
}

rainbow::cpus::shared::transform::transform(const matrix4x4& transform, const matrix4x4& inverse) :
	mInverseTransform(inverse), mTransform(transform)
{
}

rainbow::cpus::shared::transform& rainbow::cpus::shared::transform::operator*=(const transform& right)
{
	mTransform = mTransform * right.mTransform;
	mInverseTransform = right.mInverseTransform * mInverseTransform;

	return *this;
}

rainbow::cpus::shared::transform rainbow::cpus::shared::transform::operator*(const transform& right) const
{
	return transform(
		mTransform * right.mTransform,
		right.mInverseTransform * mInverseTransform
	);
}

bool rainbow::cpus::shared::transform::operator==(const transform& right) const
{
	return mTransform == right.mTransform && mInverseTransform == right.mInverseTransform;
}

bool rainbow::cpus::shared::transform::operator!=(const transform& right) const
{
	return !operator==(right);
}

rainbow::cpus::shared::surface_interaction rainbow::cpus::shared::transform::operator()(const surface_interaction& interaction) const
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

rainbow::cpus::shared::medium_interaction rainbow::cpus::shared::transform::operator()(
	const medium_interaction& interaction) const
{
	return medium_interaction(
		interaction.function,
		transform_point(*this, interaction.point),
		normalize(transform_vector(*this, interaction.wo))
	);
}

rainbow::cpus::shared::coordinate_system rainbow::cpus::shared::transform::operator()(const coordinate_system& system) const
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

rainbow::core::math::bound3 rainbow::cpus::shared::transform::operator()(const bound3& bound) const
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

rainbow::cpus::shared::ray rainbow::cpus::shared::transform::operator()(const ray& ray) const
{
	// if the transform has scale transform, the direction of will scale too
	// so the length is not right, we should scale it with length of direction
	// and normalize the direction again
	const auto direction = transform_vector(*this, ray.direction);
	
	return shared::ray(
		normalize(direction),
		transform_point(*this, ray.origin), 
		ray.length * length(direction));
}

rainbow::core::math::matrix4x4 rainbow::cpus::shared::transform::inverse_matrix() const noexcept
{
	return mInverseTransform;
}

rainbow::core::math::matrix4x4 rainbow::cpus::shared::transform::matrix() const noexcept
{
	return mTransform;
}

rainbow::cpus::shared::transform rainbow::cpus::shared::transform::inverse() const
{
	return transform(mInverseTransform, mTransform);
}

rainbow::cpus::shared::transform rainbow::cpus::shared::transform::identity()
{
	return transform();
}

rainbow::cpus::shared::transform rainbow::cpus::shared::translate(const vector3& vec)
{
	return transform(math::translate(vec), math::translate(-vec));
}

rainbow::cpus::shared::transform rainbow::cpus::shared::rotate(real angle, const vector3& axis)
{
	const auto matrix = math::rotate(angle, axis);

	return transform(matrix, math::inverse(matrix));
}

rainbow::cpus::shared::transform rainbow::cpus::shared::scale(const vector3& vec)
{
	return transform(math::scale(vec), math::scale(static_cast<real>(1) / vec));
}

rainbow::cpus::shared::transform rainbow::cpus::shared::perspective_right_hand(real fov, real width, real height, real near, real far)
{
	const auto matrix = math::perspective_right_hand<real>(fov, width, height, near, far);

	return transform(matrix, inverse(matrix));
}

rainbow::cpus::shared::transform rainbow::cpus::shared::perspective_left_hand(real fov, real width, real height, real near, real far)
{
	const auto matrix = math::perspective_left_hand<real>(fov, width, height, near, far);

	return transform(matrix, inverse(matrix));
}

rainbow::cpus::shared::transform rainbow::cpus::shared::look_at_right_hand(const vector3& origin, const vector3& target, const vector3& up)
{
	const auto matrix = math::look_at_right_hand(origin, target, up);

	return transform(matrix, inverse(matrix));
}

rainbow::cpus::shared::transform rainbow::cpus::shared::look_at_left_hand(const vector3& origin, const vector3& target, const vector3& up)
{
	const auto matrix = math::look_at_left_hand(origin, target, up);

	return transform(matrix, inverse(matrix));
}

rainbow::cpus::shared::vector3 rainbow::cpus::shared::transform_point(const transform& transform, const vector3& point)
{
	const auto v = transform.matrix() * vector4(point, 1);

	return v;
}

rainbow::cpus::shared::vector3 rainbow::cpus::shared::transform_vector(const transform& transform, const vector3& vector)
{
	return transform.matrix() * vector4(vector, 0);
}

rainbow::cpus::shared::vector3 rainbow::cpus::shared::transform_normal(const transform& transform, const vector3& normal)
{
	// before normalize it, we should convert it from vector4 to vector3
	return normalize(vector3(transpose(transform.inverse_matrix()) * vector4(normal, 0)));
}

rainbow::cpus::shared::interaction rainbow::cpus::shared::transform_interaction(const transform& transform, const interaction& interaction)
{
	return shared::interaction(
		transform_normal(transform, interaction.normal),
		transform_point(transform, interaction.point),
		transform_vector(transform, interaction.wo)
	);
}
