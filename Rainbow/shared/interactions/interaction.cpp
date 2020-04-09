#include "interaction.hpp"

rainbow::interactions::interaction::interaction(const vector3& normal, const vector3& point, const vector3& wo) :
	normal(normal), point(point), wo(wo)
{
}
