#include "disk.hpp"

#include "../shared/sample_function.hpp"

rainbow::shapes::disk::disk(real radius, real height, bool reverse_orientation) :
	shape(reverse_orientation), mHeight(height), mRadius(radius)
{
}

std::optional<rainbow::surface_interaction> rainbow::shapes::disk::intersect(const ray& ray) const
{
	const auto inner_radius = static_cast<real>(0);
	const auto outer_radius = mRadius;
	const auto phi_max = two_pi<real>();
	
	// in local space, the disk is in x-y plane
	// if the direction.z of ray is 0, it means the ray is parallel with disk(or in the same plane)
	// so it can not intersect with ray
	if (ray.direction.z == 0) return std::nullopt;

	// the hit point.z should be mHeight(the z position of disk)
	// so ray.origin.z + ray.direction.z * t = point.z = mHeight
	// t = (mHeight - ray.origin.z) / ray.direction.z
	const auto t_hit = (mHeight - ray.origin.z) / ray.direction.z;

	// the point is out of ray
	if (t_hit <= 0 || t_hit >= ray.length) return std::nullopt;

	const auto point_hit = ray.origin + ray.direction * t_hit;

	// the distance from hit point to center of disk is sqrt(x * x + y * y)
	// because the z of point is mHeight
	const auto distance_2 = point_hit.x * point_hit.x + point_hit.y * point_hit.y;

	// the point is not on the disk but on the plane
	if (distance_2 > outer_radius * outer_radius ||
		distance_2 < inner_radius * inner_radius) return std::nullopt;

	auto phi = atan2(point_hit.y, point_hit.x);

	if (phi < 0) phi = phi + two_pi<real>();
	if (phi > phi_max) return std::nullopt;

	const auto radius_hit = sqrt(distance_2);
	const auto u = phi / phi_max;
	const auto v = (outer_radius - radius_hit) / (outer_radius - inner_radius);

	const auto dp_du = vector3(-phi_max * point_hit.y, phi_max * point_hit.x, 0);
	const auto dp_dv = vector3(point_hit.x, point_hit.y, 0) * (inner_radius - outer_radius) / radius_hit;
	const auto normal = 
		reverse_orientation() ? -normalize(math::cross(dp_du, dp_dv)) : normalize(math::cross(dp_du, dp_dv));
	
	// in this version, we need set the ray.length to t_hit to avoid the ray intersect the objects far from this
	ray.length = t_hit;

	// the entity will be set when entity::intersect called
	return surface_interaction(
		nullptr,
		dp_du, dp_dv,
		normal,
		vector3(point_hit.x, point_hit.y, mHeight),
		-ray.direction,
		vector2(u, v)
	);
}

std::optional<rainbow::surface_interaction> rainbow::shapes::disk::intersect(const ray& ray, size_t index) const
{
	assert(index < mCount);

	return intersect(ray);
}

rainbow::bound3 rainbow::shapes::disk::bounding_box(const transform& transform, size_t index) const
{
	assert(index < mCount);
	
	return bounding_box(transform);
}

rainbow::bound3 rainbow::shapes::disk::bounding_box(const transform& transform) const
{
	return transform(bound3(
		vector3(-mRadius, -mRadius, mHeight),
		vector3(+mRadius, +mRadius, mHeight)
	));
}

rainbow::shapes::shape_sample rainbow::shapes::disk::sample(const vector2& sample) const
{
	const auto point_disk = concentric_sample_disk(sample);
	const auto point = vector3(point_disk.x * mRadius, point_disk.y * mRadius, mHeight);

	return shape_sample(
		interaction(vector3(0, 0, 1), point, vector3(0)),
		pdf()
	);
}

rainbow::real rainbow::shapes::disk::pdf() const
{
	return 1 / area();
}

rainbow::real rainbow::shapes::disk::area(size_t index) const noexcept
{
	assert(index < mCount);
	
	return area();
}

rainbow::real rainbow::shapes::disk::area() const noexcept
{
	return two_pi<real>() * mRadius;
}
