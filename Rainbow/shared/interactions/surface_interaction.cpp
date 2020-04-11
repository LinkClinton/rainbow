#include "surface_interaction.hpp"

rainbow::interactions::surface_interaction::surface_interaction() :
	dp_du(0), dp_dv(0), uv(0)
{
}

rainbow::interactions::surface_interaction::surface_interaction(
	const vector3& dp_du, const vector3& dp_dv,
	const vector3& point, const vector3& wo, const vector2& uv) :
	interaction(normalize(math::cross(dp_du, dp_dv)), point, wo),
	dp_du(dp_du), dp_dv(dp_dv), uv(uv)
{
	const auto shading_space_x = normalize(dp_du);
	const auto shading_space_y = math::cross(normal, shading_space_x);
	const auto shading_space_z = normal;

	shading_space = coordinate_system(
		shading_space_x,
		shading_space_y,
		shading_space_z
	);
}

rainbow::vector3 rainbow::interactions::surface_interaction::from_space_to_world(const vector3& v) const noexcept
{
	return local_to_world(shading_space, v);
}

rainbow::vector3 rainbow::interactions::surface_interaction::from_world_to_space(const vector3& v) const noexcept
{
	return world_to_local(shading_space, v);
}
