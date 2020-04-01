#include "ray.hpp"

rainbow::ray::ray() : ray(vector3(0, 0, 1), vector3(0))
{
}

rainbow::ray::ray(const vector3& direction, const vector3& origin, const real length) :
	direction(normalize(direction)), origin(origin), length(length)
{
}

rainbow::vector3 rainbow::ray::begin_point() const
{
	return origin;
}

rainbow::vector3 rainbow::ray::end_point() const
{
	return origin + direction * length;
}