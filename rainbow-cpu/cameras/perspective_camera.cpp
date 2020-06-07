#include "perspective_camera.hpp"

#include "../../rainbow-core/sample_function.hpp"

rainbow::cpus::cameras::perspective_camera::perspective_camera(
	const std::shared_ptr<cameras::film>& film,
	const bound2& screen_window,
	const transform& projective, 
	const transform& transform,
	real focus, real lens) :
	projective_camera(film, projective, transform, screen_window, focus, lens)
{
}

rainbow::cpus::cameras::perspective_camera::perspective_camera(
	const std::shared_ptr<cameras::film>& film,
	const bound2& screen_window,
	const transform& transform,
	const real fov) :
	projective_camera(film, perspective(fov,
		static_cast<real>(film->resolution().x), 
		static_cast<real>(film->resolution().y)), transform, screen_window)
{
}

rainbow::cpus::cameras::perspective_camera::perspective_camera(
	const std::shared_ptr<cameras::film>& film,
	const transform& transform, 
	const real fov) :
	projective_camera(film, perspective(fov, 
		static_cast<real>(film->resolution().x), 
		static_cast<real>(film->resolution().y)), transform, 
		bound2(vector2(-1), vector2(1)))
{
}

rainbow::cpus::shared::ray rainbow::cpus::cameras::perspective_camera::generate_ray(const camera_sample& sample) const noexcept
{
	const auto target = transform_point(mRasterToCamera, vector3(sample.position, 0));
	
	auto camera_ray = ray(normalize(target), vector3(0));

	if (mLens <= 0) return mCameraToWorld(camera_ray);

	// first, find the focus point. t = focus_distance / ray.direction.z
	// because the lens is at z = 0 and in the x-y plane
	// so the focus point is ray.origin + ray.direction * t
	// the ray that pass the center of lens does not change the direction
	const auto focus = camera_ray.origin + camera_ray.direction * (mFocus / camera_ray.direction.z);
	// sample the position of the ray start in lens
	const auto lens = concentric_sample_disk(sample.lens) * mLens;

	// generate the new ray that passed the lens
	camera_ray.origin = vector3(lens.x, lens.y, 0);
	camera_ray.direction = normalize(focus - camera_ray.origin);
	
	return mCameraToWorld(camera_ray);
}

rainbow::cpus::shared::transform rainbow::cpus::cameras::perspective_camera::perspective(real fov, real width, real height, bool right_hand)
{
	if (right_hand == true)
		return perspective_right_hand(fov, width, height);
	else
		return perspective_left_hand(fov, width, height);
}
