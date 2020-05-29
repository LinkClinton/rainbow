#include "interaction.hpp"

rainbow::interactions::interaction::interaction(const vector3& normal, const vector3& point, const vector3& wo) :
	normal(normal), point(point), wo(wo)
{
}

rainbow::ray rainbow::interactions::interaction::spawn_ray(const vector3& direction) const noexcept
{
	return ray(direction, offset_ray_origin(*this, direction));
}

rainbow::ray rainbow::interactions::interaction::spawn_ray_to(const vector3& point) const noexcept
{
	const auto ray = spawn_ray(normalize(point - this->point));

	ray.length = length(point - this->point);

	return ray;
}

rainbow::vector3 rainbow::interactions::offset_ray_origin(const interaction& interaction, const vector3& direction)
{
	// to avoid the ray intersect the surface that old ray intersect, we need offset the origin of the ray
	// the offset_scale is the value that we offset
	const auto offset_scale = ray_epsilon * (1 + max_component(abs(interaction.point)));

	// get the direction we offset
	auto offset = offset_scale * interaction.normal;

	// if the direction of ray and normal of surface are not in the same hemisphere
	// we need invert the offset
	if (dot(interaction.normal, direction) < 0) offset = -offset;

	auto offset_origin = interaction.point + offset;

	// compute the next float point at offset_origin
	offset_origin.x = std::nextafter(offset_origin.x, offset.x > 0 ? std::numeric_limits<real>::max() : std::numeric_limits<real>::min());
	offset_origin.y = std::nextafter(offset_origin.y, offset.y > 0 ? std::numeric_limits<real>::max() : std::numeric_limits<real>::min());
	offset_origin.z = std::nextafter(offset_origin.z, offset.z > 0 ? std::numeric_limits<real>::max() : std::numeric_limits<real>::min());
	
	return offset_origin;
}
