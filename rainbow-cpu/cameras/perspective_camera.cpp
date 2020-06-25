#include "perspective_camera.hpp"

#include "../../rainbow-core/sample_function.hpp"

rainbow::cpus::cameras::perspective_camera::perspective_camera(
	const std::shared_ptr<cameras::film>& film,
	const camera_system& system,
	const bound2& screen_window,
	const transform& projective, 
	const transform& transform,
	real focus, real lens) :
	projective_camera(film, system, projective, transform, screen_window, focus, lens)
{
	auto min = transform_point(mRasterToCamera, vector3(0, 0, 0));
	auto max = transform_point(mRasterToCamera, vector3(mFilm->resolution(), 0));

	min = min / min.z;
	max = max / max.z;

	mArea = math::abs((max.x - min.x) * (max.y - min.y));
}

std::tuple<rainbow::cpus::shared::spectrums::spectrum, rainbow::core::vector2> rainbow::cpus::cameras::perspective_camera::evaluate(const ray& ray) const
{
	// if the coordinate system is left hand, the forward axis should be (0, 0, 1)
	// if the coordinate system is right hand, the forward axis should be (0, 0, -1)
	const auto forward = mCameraSystem == camera_system::right_hand ? vector3(0, 0, -1) : vector3(0, 0, 1);
	const auto cos_theta = dot(ray.direction, transform_vector(mCameraToWorld, forward));

	// if the cos_theta is 0, means the ray and forward is not in the same hemisphere
	// in other words, the ray is invisible, so we just return 0
	if (cos_theta <= 0) return { spectrum(0), vector2(0) };

	// if the lens is not 0, the z value of focus point should be focus. otherwise, it should be 1.
	// and the t of point on the ray and focus plane should be z value of focus point / cos_theta
	const auto t = (mLens > 0 ? mFocus : 1) / cos_theta;

	// the focus point in raster space, we first find the focus point in world space(ray.origin + ray.direction * t)
	// and transform it from world space to camera space
	// and transform it from camera space to raster space
	const auto focus =
		transform_point(mRasterToCamera.inverse(),
			transform_point(mCameraToWorld.inverse(), ray.origin + ray.direction * t));

	const auto pixels_bound = mFilm->pixels_bound();

	// if the focus point is not on the film, we just return 0
	if (focus.x < pixels_bound.min.x || focus.y >= pixels_bound.max.x ||
		focus.y < pixels_bound.min.y || focus.y >= pixels_bound.max.y)
		return { spectrum(0), vector2(0) };

	const auto lens_area = mLens != 0 ? pi<real>() * mLens * mLens : 1;
	const auto cos_theta_2 = cos_theta * cos_theta;

	return { spectrum(1 / (mArea * lens_area * cos_theta_2 * cos_theta_2)), vector2(focus.x, focus.y) };
}

rainbow::cpus::cameras::camera_sample rainbow::cpus::cameras::perspective_camera::sample(const interaction& reference,
	const vector2& sample) const
{
	// sample the point on the lens and transform it from local space to world space
	const auto lens_local_space = mLens * concentric_sample_disk(sample);
	const auto lens_world_space = transform_point(mCameraToWorld, vector3(lens_local_space, 0));
	const auto normal = transform_normal(mCameraToWorld, vector3(0, 0, 1));

	const auto interaction = shared::interaction(normal, lens_world_space, vector3());
	const auto distance = length(interaction.point - reference.point);
	const auto wi = normalize(interaction.point - reference.point);

	const auto lens_area = mLens != 0 ? pi<real>() * mLens * mLens : 1;
	const auto pdf = (distance * distance) / (math::abs(dot(interaction.normal, wi)) * lens_area);

	const auto [value, point] = evaluate(interaction.spawn_ray(-wi));

	return camera_sample(value, wi, point, pdf);
}

rainbow::cpus::shared::ray rainbow::cpus::cameras::perspective_camera::sample(const vector2& position, const vector2& sample) const
{
	const auto target = transform_point(mRasterToCamera, vector3(position, 0));
	
	auto camera_ray = ray(normalize(target), vector3(0));

	if (mLens <= 0) return mCameraToWorld(camera_ray);

	// first, find the focus point. t = focus_distance / ray.direction.z
	// because the lens is at z = 0 and in the x-y plane
	// so the focus point is ray.origin + ray.direction * t
	// the ray that pass the center of lens does not change the direction
	const auto focus = camera_ray.origin + camera_ray.direction * (mFocus / camera_ray.direction.z);
	// sample the position of the ray start in lens
	const auto lens = concentric_sample_disk(sample) * mLens;

	// generate the new ray that passed the lens
	camera_ray.origin = vector3(lens.x, lens.y, 0);
	camera_ray.direction = normalize(focus - camera_ray.origin);
	
	return mCameraToWorld(camera_ray);
}

std::tuple<rainbow::core::real, rainbow::core::real> rainbow::cpus::cameras::perspective_camera::pdf(const ray& ray) const
{
	// if the coordinate system is left hand, the forward axis should be (0, 0, 1)
	// if the coordinate system is right hand, the forward axis should be (0, 0, -1)
	const auto forward = mCameraSystem == camera_system::right_hand ? vector3(0, 0, -1) : vector3(0, 0, 1);
	const auto cos_theta = dot(ray.direction, transform_vector(mCameraToWorld, forward));

	// if the cos_theta is 0, means the ray and forward is not in the same hemisphere
	// in other words, the ray is invisible, so we just return 0
	if (cos_theta <= 0) return { static_cast<real>(0), static_cast<real>(0) };

	// if the lens is not 0, the z value of focus point should be focus. otherwise, it should be 1.
	// and the t of point on the ray and focus plane should be z value of focus point / cos_theta
	const auto t = (mLens > 0 ? mFocus : 1) / cos_theta;

	// the focus point in raster space, we first find the focus point in world space(ray.origin + ray.direction * t)
	// and transform it from world space to camera space
	// and transform it from camera space to raster space
	const auto focus =
		transform_point(mRasterToCamera.inverse(),
			transform_point(mCameraToWorld.inverse(), ray.origin + ray.direction * t));

	const auto pixels_bound = mFilm->pixels_bound();

	// if the focus point is not on the film, we just return 0
	if (focus.x < pixels_bound.min.x || focus.y >= pixels_bound.max.x ||
		focus.y < pixels_bound.min.y || focus.y >= pixels_bound.max.y)
		return { static_cast<real>(0), static_cast<real>(0) };

	const auto lens_area = mLens != 0 ? pi<real>() * mLens * mLens : 1;

	// the first is pdf_position, the second is pdf_direction
	return { 1 / lens_area, 1 / (mArea * cos_theta * cos_theta * cos_theta) };
}

rainbow::cpus::shared::transform rainbow::cpus::cameras::perspective_camera::perspective(real fov, real width, real height, bool right_hand)
{
	if (right_hand == true)
		return perspective_right_hand(fov, width, height);
	else
		return perspective_left_hand(fov, width, height);
}
