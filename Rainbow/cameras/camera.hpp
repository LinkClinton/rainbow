#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/transform.hpp"
#include "../shared/ray.hpp"

#include "film.hpp"

namespace rainbow {

	namespace cameras {

		class camera : public interfaces::noncopyable {
		public:
			explicit camera(
				const std::shared_ptr<film>& film,
				const transform& transform);

			~camera() = default;

			virtual ray generate_ray(const vector2& position) const noexcept = 0;

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
				const bound2& screen_window);

			~projective_camera() = default;
		protected:
			transform mCameraToScreen;
			transform mScreenToRaster;
			transform mRasterToCamera;
			transform mRasterToScreen;
		};
	}
}
