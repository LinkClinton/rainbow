#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/transform.hpp"
#include "../shared/ray.hpp"

#include "film.hpp"

namespace rainbow::cpus::cameras {

	using namespace shared;
	
	struct camera_sample {
		spectrum value = spectrum(0);
		vector3 wi = vector3(0);
		vector2 point = vector2(0);
		real pdf = 0;

		camera_sample() = default;

		camera_sample(const spectrum& value, const vector3& wi, const vector2& point, real pdf);
	};

	enum class camera_system : uint32 {
		right_hand = 0,
		left_hand = 1
	};
	
	class camera : public interfaces::noncopyable {
	public:
		explicit camera(
			const std::shared_ptr<film>& film,
			const camera_system& system,
			const transform& transform);

		~camera() = default;

		virtual std::tuple<spectrum, vector2> evaluate(const ray& ray) const = 0;
		
		virtual camera_sample sample(const interaction& reference, const vector2& sample) const = 0;
		
		virtual ray sample(const vector2& position, const vector2& sample) const = 0;

		virtual std::tuple<real, real> pdf(const ray& ray) const = 0;
		
		std::shared_ptr<film> film() const noexcept;
	protected:
		std::shared_ptr<cameras::film> mFilm;

		camera_system mCameraSystem;
		
		transform mCameraToWorld;
	};

	class projective_camera : public camera {
	public:
		explicit projective_camera(
			const std::shared_ptr<cameras::film>& film,
			const camera_system& system,
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