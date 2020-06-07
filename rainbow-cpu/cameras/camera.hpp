#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/transform.hpp"
#include "../shared/ray.hpp"

#include "film.hpp"

namespace rainbow::cpus::cameras {

	using namespace shared;
	
	struct camera_sample {
		vector2 position;
		vector2 lens;

		camera_sample() = default;

		camera_sample(const vector2& position, const vector2& lens);
	};

	class camera : public interfaces::noncopyable {
	public:
		explicit camera(
			const std::shared_ptr<film>& film,
			const transform& transform);

		~camera() = default;

		virtual ray generate_ray(const camera_sample& sample) const noexcept = 0;

		std::shared_ptr<film> film() const noexcept;
	protected:
		std::shared_ptr<cameras::film> mFilm;

		transform mCameraToWorld;
	};

	class projective_camera : public camera {
	public:
		explicit projective_camera(
			const std::shared_ptr<cameras::film>& film,
			const transform& projective,
			const transform& transform,
			const bound2& screen_window,
			real focus = 1e6, real lens = 0);

		~projective_camera() = default;
	protected:
		transform mCameraToScreen;
		transform mScreenToRaster;
		transform mRasterToCamera;
		transform mRasterToScreen;

		real mFocus = 1e6;
		real mLens = 0;
	};
}