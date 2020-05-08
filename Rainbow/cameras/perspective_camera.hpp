#pragma once

#include "camera.hpp"

namespace rainbow {

	namespace cameras {

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

			ray generate_ray(const vector2& position) const noexcept override;
		};
		
	}
}
