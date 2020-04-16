#pragma once

#include "../math/math.hpp"
#include "../ray.hpp"

namespace rainbow {

	using namespace math;
	
	namespace interactions {

		struct interaction {
			vector3 normal;
			vector3 point;
			vector3 wo;
			
			interaction() = default;

			interaction(const vector3& normal, const vector3& point, const vector3& wo);

			ray spawn_ray(const vector3& direction) const noexcept;

			ray spawn_ray_to(const vector3& point) const noexcept;
		};

		vector3 offset_ray_origin(const interaction& interaction, const vector3& direction);
	}
}
