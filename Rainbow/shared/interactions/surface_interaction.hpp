#pragma once

#include "../coordinate_system.hpp"

#include "interaction.hpp"

namespace rainbow {
	
	namespace interactions {

		struct surface_interaction : public interaction {
			coordinate_system shading_space;
			
			vector3 dp_du, dp_dv;
			vector2 uv;
			
			surface_interaction();

			surface_interaction(
				const vector3& dp_du, const vector3& dp_dv,
				const vector3& point, const vector3& wo,
				const vector2& uv);

			vector3 from_space_to_world(const vector3& v) const noexcept;
			
			vector3 from_world_to_space(const vector3& v) const noexcept;
		};
		
	}
}
