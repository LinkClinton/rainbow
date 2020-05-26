#pragma once

#include "camera.hpp"

namespace rainbow {

	namespace cameras {

		class perspective_camera final : public projective_camera {
		public:
			explicit perspective_camera(
				const std::shared_ptr<cameras::film>& film,
				const bound2& screen_window,
				const transform& transform,
				const real fov);

			explicit perspective_camera(
				const std::shared_ptr<cameras::film>& film,
				const bound2& screen_window,
				const transform& projective,
				const transform& transform,
				real focus = 1e6, real lens = 0);
			
			explicit perspective_camera(
				const std::shared_ptr<cameras::film>& film,
				const transform& transform,
				const real fov);
			
			ray generate_ray(const camera_sample& sample) const noexcept override;
		private:
			static transform perspective(real fov, real width, real height, bool right_hand = true);
		};
		
	}
}
