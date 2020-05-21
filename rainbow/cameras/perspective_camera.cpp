#include "perspective_camera.hpp"

rainbow::cameras::perspective_camera::perspective_camera(
	const std::shared_ptr<cameras::film>& film,
	const bound2& screen_window,
	const transform& projective, 
	const transform& transform) :
	projective_camera(film, projective, transform, screen_window)
{
}

rainbow::cameras::perspective_camera::perspective_camera(
	const std::shared_ptr<cameras::film>& film,
	const bound2& screen_window,
	const transform& transform,
	const real fov) :
	projective_camera(film, perspective(fov,
		static_cast<real>(film->resolution().x), 
		static_cast<real>(film->resolution().y)), transform, screen_window)
{
}

rainbow::cameras::perspective_camera::perspective_camera(
	const std::shared_ptr<cameras::film>& film,
	const transform& transform, 
	const real fov) :
	projective_camera(film, perspective(fov, 
		static_cast<real>(film->resolution().x), 
		static_cast<real>(film->resolution().y)), transform, 
		bound2(vector2(-1), vector2(1)))
{
}

rainbow::ray rainbow::cameras::perspective_camera::generate_ray(const vector2& position) const noexcept
{
	const auto target = transform_point(mRasterToCamera, vector3(position, 0));

	return mCameraToWorld(ray(normalize(target), vector3(0)));
}

rainbow::transform rainbow::cameras::perspective_camera::perspective(real fov, real width, real height, bool right_hand)
{
	if (right_hand == true)
		return perspective_right_hand(fov, width, height);
	else
		return perspective_left_hand(fov, width, height);
}
