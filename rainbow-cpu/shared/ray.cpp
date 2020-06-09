#include "ray.hpp"

rainbow::cpus::shared::ray::ray() : ray(vector3(0, 0, 1), vector3(0))
{
}

rainbow::cpus::shared::ray::ray(const vector3& direction, const vector3& origin, real length) :
	direction(normalize(direction)), origin(origin), length(length)
{
}

rainbow::core::math::vector3 rainbow::cpus::shared::ray::begin_point() const
{
	return origin;
}

rainbow::core::math::vector3 rainbow::cpus::shared::ray::end_point() const
{
	return origin + direction * length;
}

rainbow::cpus::shared::ray rainbow::cpus::shared::ray::reverse() const
{
	assert(length != std::numeric_limits<real>::max());
	
	return ray(-direction, origin + direction * length, length);
}
