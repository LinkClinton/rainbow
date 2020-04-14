#include "disk.hpp"

rainbow::disk::disk(
	const std::shared_ptr<materials::material>& material, 
	const rainbow::transform& transform, real radius, real height) :
	shape(material, transform), mHeight(height), mRadius(radius)
{
}

std::optional<rainbow::surface_interaction> rainbow::disk::intersect(const ray& ray)
{
	const auto ray_local = mWorldToLocal(ray);

	const auto inner_radius = static_cast<real>(0);
	const auto outer_radius = mRadius;
	const auto phi_max = two_pi<real>();
	
	// in local space, the disk is in x-y plane
	// if the direction.z of ray is 0, it means the ray is parallel with disk(or in the same plane)
	// so it can not intersect with ray
	if (ray_local.direction.z == 0) return std::nullopt;

	// the hit point.z should be mHeight(the z position of disk)
	// so ray.origin.z + ray.direction.z * t = point.z = mHeight
	// t = (mHeight - ray.origin.z) / ray.direction.z
	const auto t_hit = (mHeight - ray_local.origin.z) / ray_local.direction.z;

	// the point is out of ray
	if (t_hit <= 0 || t_hit >= ray_local.length) return std::nullopt;

	const auto point_hit = ray_local.origin + ray_local.direction * t_hit;

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

	// in this version, we need set the ray.length to t_hit to avoid the ray intersect the objects far from this
	ray.length = t_hit;

	return mLocalToWorld(surface_interaction(
		shared_from_this(),
		dp_du, dp_dv, 
		vector3(point_hit.x, point_hit.y, mHeight),
		-ray_local.direction,
		vector2(u, v)
	));
}
