#pragma once

#include "camera.hpp"

namespace rainbow {

	namespace cameras {

		enum class coordinate_system : uint32 {
			right_hand = 0, left_hand = 1
		};
		
		class perspective_camera final : public projective_camera {
		public:
			explicit perspective_camera(
				const std::shared_ptr<cameras::film>& film,
				const transform& transform,
				const bound2& screen_window,
				const real fov);

			explicit perspective_camera(
				const std::shared_ptr<cameras::film>& film,
				const transform& transform,
				const real fov);

			explicit perspective_camera(
				const std::shared_ptr<cameras::film>& film,
				const transform& transform,
				const coordinate_system& system,
				const real fov);
			
			ray generate_ray(const vector2& position) const noexcept override;
		private:
			static transform perspective(real fov, real width, real height, const coordinate_system& system);
			
			coordinate_system mCoordinateSystem = coordinate_system::right_hand;
		};
		
	}
}
