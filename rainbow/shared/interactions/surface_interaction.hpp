#pragma once

#include "../coordinate_system.hpp"

#include "interaction.hpp"

namespace rainbow {

	namespace scenes {
		class entity;
	}
	
	using namespace scenes;
	
	namespace interactions {

		struct surface_interaction : public interaction {
			coordinate_system shading_space;

			std::shared_ptr<const entity> entity;
			size_t index;
			
			vector3 dp_du, dp_dv;
			vector2 uv;
			
			surface_interaction();

			surface_interaction(
				const std::shared_ptr<const scenes::entity>& entity, size_t index = 0);
			
			surface_interaction(
				const std::shared_ptr<const scenes::entity>& entity,
				const vector3& dp_du, const vector3& dp_dv,
				const vector3& normal, const vector3& point,
				const vector3& wo, const vector2& uv,
				size_t index = 0);

			surface_interaction(
				const std::shared_ptr<const scenes::entity>& entity,
				const coordinate_system& shading_space,
				const vector3& dp_du, const vector3& dp_dv,
				const vector3& normal, const vector3& point,
				const vector3& wo, const vector2& uv,
				size_t index = 0);

			vector3 from_space_to_world(const vector3& v) const noexcept;
			
			vector3 from_world_to_space(const vector3& v) const noexcept;
		};
		
	}
}