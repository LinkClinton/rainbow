#pragma once

#include "../../../rainbow-core/math/math.hpp"
#include "../ray.hpp"

namespace rainbow::cpus::shared::interactions {

	using namespace math;

	struct interaction {
		vector3 normal = vector3(0);
		vector3 point = vector3(0);
		vector3 wo = vector3(0);

		interaction() = default;

		interaction(const vector3& normal, const vector3& point, const vector3& wo);

		interaction(const vector3& point);
		
		ray spawn_ray(const vector3& direction) const noexcept;

		ray spawn_ray_to(const vector3& point) const noexcept;
	};

	vector3 offset_ray_origin(const interaction& interaction, const vector3& direction);

}