#include "sphere.hpp"

#include "../../rainbow-core/shading_function.hpp"
#include "../../rainbow-core/sample_function.hpp"

using namespace rainbow::cpus::shared::interactions;

rainbow::cpus::shapes::sphere::sphere(real radius, bool reverse_orientation) : shape(reverse_orientation), mRadius(radius)
{
}

std::optional<surface_interaction> rainbow::cpus::shapes::sphere::intersect(const ray& ray, size_t index) const
{
	return intersect(ray);
}

std::optional<surface_interaction> rainbow::cpus::shapes::sphere::intersect(const ray& ray) const
{
	// a = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z
	// b = 2 * (direction.x * origin.x + direction.y * origin.y + direction.z * origin.z)
	// c = origin.x * origin.x + origin.y * origin.y + origin.z * origin.z - radius * radius
	const auto a = dot(ray.direction, ray.direction);
	const auto b = dot(ray.direction, ray.origin) * 2;
	const auto c = dot(ray.origin, ray.origin) - mRadius * mRadius;

	real t0, t1, t_hit;

	// solve the equation to get the point ray intersect sphere
	if (!solve_quadratic_equation(a, b, c, &t0, &t1)) return std::nullopt;

	// the point should on the ray
	if (t0 > ray.length || t1 <= 0) return std::nullopt;

	if (t0 <= 0) {
		if (t1 > ray.length) return std::nullopt;
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
	const auto theta_min = acos(static_cast<real>(-1));
	const auto theta_max = acos(static_cast<real>(1));
	
	// parametric representation of sphere hit
	const auto theta = acos(clamp(point_hit.z / mRadius, static_cast<real>(-1), static_cast<real>(1)));
	const auto u = phi / phi_max;
	const auto v = (theta - theta_min) / (theta_max - theta_min);

	// compute the dp_du, dp_dv
	const auto radius = sqrt(point_hit.x * point_hit.x + point_hit.y * point_hit.y);
	const auto inv_radius = 1 / radius;
	const auto cos_phi = point_hit.x * inv_radius;
	const auto sin_phi = point_hit.y * inv_radius;

	const auto dp_du = vector3(-phi_max * point_hit.y, phi_max * point_hit.x, 0);
	const auto dp_dv = vector3(point_hit.z * cos_phi, point_hit.z * sin_phi, -mRadius * sin(theta)) * (theta_max - theta_min);
	const auto normal = 
		reverse_orientation() ? -normalize(math::cross(dp_du, dp_dv)) : normalize(math::cross(dp_du, dp_dv));
	
	// in this version, we need set the ray.length to t_hit to avoid the ray intersect the objects far from this
	ray.length = t_hit;

	// the normal of surface is indicate the outside of shape
	// the entity will be set when entity::intersect called
	return surface_interaction(
		nullptr,
		dp_du, dp_dv, normal, point_hit, -ray.direction,
		vector2(u, v)
	);
}

rainbow::core::math::bound3 rainbow::cpus::shapes::sphere::bounding_box(const transform& transform, size_t index) const
{
	return bounding_box(transform);
}

rainbow::core::math::bound3 rainbow::cpus::shapes::sphere::bounding_box(const transform& transform) const
{
	const auto center = transform_point(transform, vector3(0));
	const auto point = transform_point(transform, vector3(0, 0, mRadius));
	const auto radius = length(point - center);
	
	return bound3(center - radius, center + radius);
}

rainbow::cpus::shapes::shape_sample rainbow::cpus::shapes::sphere::sample(const shape_instance_properties& properties, 
	const interaction& reference, const vector2& sample) const
{
	const auto center = vector3(0);

	// the reference point is in the inside of sphere
	// we will sample it using the sample function without reference point
	if (distance_squared(center, reference.point) <= mRadius * mRadius) {
		auto shape_sample = sphere::sample(properties, sample);
		auto wi = shape_sample.interaction.point - reference.point;

		if (length_squared(wi) == 0) return {};

		wi = normalize(wi);

		// notice : convert the pdf from area measure to solid angle measure
		shape_sample.pdf = shape_sample.pdf * 
			distance_squared(reference.point, shape_sample.interaction.point) /
			abs(dot(shape_sample.interaction.normal, -wi));

		if (isinf(shape_sample.pdf)) return {};

		return shape_sample;
	}
	
	// the reference point is in the outside of sphere
	// we will sample a subtended cone to get the point

	const auto distance = math::distance(reference.point, center);
	const auto inv_distance = 1 / distance;

	// theta_max is the angle between vector (reference.point - center) and vector that tangent with sphere and pass reference.point
	// we will use the sample.x to sample the theta value in the cone 
	const auto sin_theta_max = mRadius * inv_distance;
	const auto sin_theta_max_2 = sin_theta_max * sin_theta_max;
	const auto inv_sin_theta_max = 1 / sin_theta_max;
	const auto cos_theta_max = sqrt(max(static_cast<real>(0), 1 - sin_theta_max_2));

	// sample cos theta, lerp(cos_theta_max, 1, sample.x) = cos_theta_max * sample.x + (1 - sample.x) * 1
	// cos_theta_max * sample.x + (1 - sample.x) * 1 = (cos_theta_max - 1) * sample.x + 1
	auto cos_theta = (cos_theta_max - 1) * sample.x + 1;
	auto sin_theta_2 = 1 - cos_theta * cos_theta;

	// sin^2 degree(1.5) = 0.00068523, so we just test theta < degree(1.5)
	if (sin_theta_max_2 < 0.00068523) {
		sin_theta_2 = sin_theta_max_2 * sample.x;
		cos_theta = sqrt(1 - sin_theta_2);
	}

	// dc = distance(reference.point, center)
	// ds = dc * cos_theta - sqrt(r^2 - dc^2 * sin_theta_2)
	// ds^2 = dc^2 * cos_theta_2 + r^2 - dc^2 * sin_theta_2 - 2 * dc * cos_theta * sqrt(r^2 - dc^2 * sin_theta_2)
	// sin_theta_max = r / dc, inv_sin_theta_max = dc / r
	// cos_alpha = (dc^2 + r^2 - ds^2) / (2 * dc * r)
	// cos_alpha = (dc^2 + r^2 - dc^2 * cos_theta_2 - r^2 + dc^2 * sin_theta_2 + 2 * dc * cos_theta * sqrt(r^2 - dc^2 * sin_theta_2)) / (2 * dc * r)
	// cos_alpha = (dc^2 * (1 - cos_theta_2 + sin_theta_2) + 2 * dc * cos_theta * sqrt(r^2 - dc^2 * sin_theta_2)) / (2 * dc * r)
	// cos_alpha = (dc * (1 - cos_theta_2 + sin_theta_2) + 2 * cos_theta * sqrt(r^2 - dc^2 * sin_theta_2)) / (2 * r)
	// cos_alpha = (dc / r * (1 - cos_theta_2 + sin_theta_2) + 2 * cos_theta * sqrt(1 - (dc / r)^2 * sin_theta_2)) / 2
	// cos_alpha = (inv_sin_theta_max * (1 - cos_theta_2 + sin_theta_2) / 2 + cos_theta * sqrt(1 - inv_sin_theta_max * inv_sin_theta_max * sin_theta_2))
	// cos_alpha = (inv_sin_theta_max * (sin_theta_2 + cos_theta_2 - cos_theta_2 + sin_theta_2) / 2 + ...keep unchanged.
	// cos_alpha = (inv_sin_theta_max * sin_theta_2 + cos_theta * sqrt(1 - inv_sin_theta_max * inv_sin_theta_max * sin_theta_2))
	// ... wtf 
	const auto cos_alpha = inv_sin_theta_max * sin_theta_2 + cos_theta * 
		sqrt(max(static_cast<real>(0), 1 - inv_sin_theta_max * inv_sin_theta_max * sin_theta_2));
	const auto sin_alpha = sqrt(max(static_cast<real>(0), 1 - cos_alpha * cos_alpha));
	const auto phi = sample.y * two_pi<real>();
	
	// build a coordinate system with z = normalize(reference.point - center)
	const auto local_system = coordinate_system(reference.point - center);

	// transform the normal from local system to world system
	// using the spherical direction to find the normal
	const auto normal = normalize(local_to_world(local_system, spherical_direction(sin_alpha, cos_alpha, phi)));
	const auto point = center + mRadius * normal;

	return shape_sample(
		interaction(normal, point, vector3(0)),
		uniform_sample_cone_pdf(cos_theta_max)
	);
}

rainbow::cpus::shapes::shape_sample rainbow::cpus::shapes::sphere::sample(const shape_instance_properties& properties, const vector2& sample) const
{
	const auto point = vector3(0) + mRadius * uniform_sample_sphere(sample);

	return shape_sample(
		interaction(normalize(point), point, vector3(0)),
		pdf(properties)
	);
}

rainbow::core::real rainbow::cpus::shapes::sphere::pdf(const shape_instance_properties& properties, 
	const interaction& reference, const vector3& wi) const
{
	const auto center = vector3(0);

	if (distance_squared(center, reference.point) <= mRadius * mRadius)
		return pdf(properties);

	const auto sin_theta_max_2 = mRadius * mRadius / distance_squared(reference.point, center);
	const auto cos_theta_max = sqrt(max(static_cast<real>(0), 1 - sin_theta_max_2));

	return uniform_sample_cone_pdf(cos_theta_max);
}

rainbow::core::real rainbow::cpus::shapes::sphere::pdf(const shape_instance_properties& properties) const
{
	// the transform of entity may have scale component
	// the area of world space is not equal to the area of local space
	// so we will use shape_instance_properties::area(the area in world space)
	return 1 / properties.area;
}

real rainbow::cpus::shapes::sphere::area(const transform& transform, size_t index) const noexcept
{
	return area(transform);
}

real rainbow::cpus::shapes::sphere::area(const transform& transform) const noexcept
{
	const auto center = transform_point(transform, vector3(0));
	const auto point = transform_point(transform, vector3(0, 0, mRadius));
	const auto radius = length(point - center);

	return 4 * pi<real>() * radius * radius;
}

rainbow::core::real rainbow::cpus::shapes::sphere::area(size_t index) const noexcept
{
	return area();
}

rainbow::core::real rainbow::cpus::shapes::sphere::area() const noexcept
{
	return 4 * pi<real>() * mRadius * mRadius;
}

void rainbow::cpus::shapes::sphere::build_accelerator()
{
}
