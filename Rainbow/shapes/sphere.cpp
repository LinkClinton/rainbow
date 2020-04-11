#include "sphere.hpp"

rainbow::shapes::sphere::sphere(const rainbow::transform& transform, real radius) :
	shape(transform), mRadius(radius)
{
}

std::optional<rainbow::surface_interaction> rainbow::shapes::sphere::intersect(const ray& ray)
{
	const auto ray_local = mWorldToLocal(ray);
	
	// a = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z
	// b = 2 * (direction.x * origin.x + direction.y * origin.y + direction.z * origin.z)
	// c = origin.x * origin.x + origin.y * origin.y + origin.z * origin.z - radius * radius
	const auto a = dot(ray_local.direction, ray_local.direction);
	const auto b = dot(ray_local.direction, ray_local.origin) * 2;
	const auto c = dot(ray_local.origin, ray_local.origin) - mRadius * mRadius;

	real t0, t1, t_hit;

	// solve the equation to get the point ray intersect sphere
	if (!solve_quadratic_equation(a, b, c, &t0, &t1)) return std::nullopt;

	// the point should on the ray
	if (t0 > ray_local.length || t1 <= 0) return std::nullopt;

	if (t0 <= 0) {
		if (t1 > ray_local.length) return std::nullopt;
		else t_hit = t1;
	}
	else t_hit = t0;

	auto point_hit = ray.origin + ray.direction * t_hit;

	// if the point on the sphere, the distance of point should be radius
	// so mRadius / length(point_hit) should be 1
	point_hit = point_hit * mRadius / length(point_hit);

	// if point is in the z-axis, the phi of point is meaningless
	if (point_hit.x == 0 && point_hit.y == 0) point_hit.x = 1e-5f * mRadius;

	auto phi = atan2(point_hit.y, point_hit.x);
	
	if (phi < 0) phi = phi + two_pi<real>();

	// we use phi_max, theta_min and theta_max to define the part of sphere
	// now, we only use the whole sphere, so the phi_max should be 2 * pi
	// theta should be the range [-1, 1]
	const auto phi_max = two_pi<real>();
	const auto theta_min = static_cast<real>(-1);
	const auto theta_max = static_cast<real>(1);
	
	// parametric representation of sphere hit
	const auto theta = acos(clamp(point_hit.z / mRadius, static_cast<real>(-1), static_cast<real>(1)));
	const auto u = phi / two_pi<real>();
	const auto v = (theta - theta_min) / (theta_max - theta_min);

	// compute the dp_du, dp_dv
	const auto radius = sqrt(point_hit.x * point_hit.x + point_hit.y * point_hit.y);
	const auto inv_radius = 1 / radius;
	const auto cos_phi = point_hit.x * inv_radius;
	const auto sin_phi = point_hit.y * inv_radius;

	const auto dp_du = vector3(-phi_max * point_hit.y, phi_max * point_hit.x, 0);
	const auto dp_dv = vector3(point_hit.z * cos_phi, point_hit.z * sin_phi, -mRadius * sin(theta)) * (theta_max - theta_min);

	return surface_interaction(
		dp_du, dp_dv, point_hit, -ray_local.direction,
		vector2(u, v)
	);
}
