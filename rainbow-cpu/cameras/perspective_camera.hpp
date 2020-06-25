#pragma once

#include "camera.hpp"

namespace rainbow::cpus::cameras {

	class perspective_camera final : public projective_camera {
	public:
		explicit perspective_camera(
			const std::shared_ptr<cameras::film>& film,
			const camera_system& system,
			const bound2& screen_window,
			const transform& projective,
			const transform& transform,
			real focus = 1e6, real lens = 0);

		std::tuple<spectrum, vector2> evaluate(const ray& ray) const override;
		
		camera_sample sample(const interaction& reference, const vector2& sample) const override;

		ray sample(const vector2& position, const vector2& sample) const override;
		
		std::tuple<real, real> pdf(const ray& ray) const override;
	private:
		static transform perspective(real fov, real width, real height, bool right_hand = true);

		real mArea;
	};

}