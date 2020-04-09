#include "coordinate_system.hpp"

rainbow::coordinate_system::coordinate_system() :
	coordinate_system(vector3(1, 0, 0), vector3(0, 1, 0), vector3(0, 0, 1))
{
}

rainbow::coordinate_system::coordinate_system(const std::array<vector3, 3>& axes) : axes(axes)
{
	for (auto& axis : this->axes) axis = normalize(axis);
}

rainbow::coordinate_system::coordinate_system(const vector3& x, const vector3& y, const vector3& z) : axes({ x, y, z })
{
}

rainbow::vector3 rainbow::coordinate_system::x() const noexcept
{
	return axes[0];
}

rainbow::vector3 rainbow::coordinate_system::y() const noexcept
{
	return axes[1];
}

rainbow::vector3 rainbow::coordinate_system::z() const noexcept
{
	return axes[2];
}

rainbow::vector3 rainbow::world_to_local(const coordinate_system& system, const vector3& world_vector)
{
	// we only transform the vector in world space to local space the system defined
	// the dot of point and axis is the length that the point project to axis
	return vector3(dot(world_vector, system.x()), dot(world_vector, system.y()), dot(world_vector, system.z()));
}

rainbow::vector3 rainbow::local_to_world(const coordinate_system& system, const vector3& local_vector)
{
	return vector3(
		system.x().x * local_vector.x + system.y().x * local_vector.y + system.z().x * local_vector.z,
		system.x().y * local_vector.x + system.y().y * local_vector.y + system.z().y * local_vector.z,
		system.x().z * local_vector.x + system.y().z * local_vector.y + system.z().z * local_vector.z
	);
}
