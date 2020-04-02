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
		right.mInverseTransform * mTransform
	);
}

rainbow::vector4 rainbow::transform::operator()(const vector2& vec) const
{
	return mTransform * vector4(vec, 0, 1);
}

rainbow::vector4 rainbow::transform::operator()(const vector3& vec) const
{
	return mTransform * vector4(vec, 1);
}

rainbow::vector4 rainbow::transform::operator()(const vector4& vec) const
{
	return mTransform * vec;
}

rainbow::ray rainbow::transform::operator()(const ray& ray) const
{
	return rainbow::ray((*this)(ray.direction), (*this)(ray.origin), ray.length);
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

	return transform(matrix, math::transpose(matrix));
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
