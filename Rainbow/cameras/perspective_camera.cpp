#include "perspective_camera.hpp"

rainbow::cameras::perspective_camera::perspective_camera(
	const std::shared_ptr<cameras::film>& film,
	const transform& transform,
	const bound2& screen_window,
	const real fov) : projective_camera(film, perspective(fov), transform, screen_window)
{
}

rainbow::ray rainbow::cameras::perspective_camera::generate_ray(const vector2& position) const noexcept
{
	const auto target = mRasterToCamera(vector3(position, 0));

	return mCameraToWorld(ray(normalize(target), vector3(0)));
}
