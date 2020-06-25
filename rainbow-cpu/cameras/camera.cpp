#include "camera.hpp"

rainbow::cpus::cameras::camera_sample::camera_sample(const spectrum& value, const vector3& wi, const vector2& point, real pdf) :
	value(value), wi(wi), point(point), pdf(pdf)
{
}

rainbow::cpus::cameras::camera::camera(
	const std::shared_ptr<cameras::film>& film, const camera_system& system, const transform& transform) :
	mFilm(film), mCameraSystem(system), mCameraToWorld(transform)
{
	
}

std::shared_ptr<rainbow::cpus::cameras::film> rainbow::cpus::cameras::camera::film() const noexcept
{
	return mFilm;
}

rainbow::cpus::cameras::projective_camera::projective_camera(
	const std::shared_ptr<cameras::film>& film,
	const camera_system& system,
	const transform& projective,
	const transform& transform, 
	const bound2& screen_window,
	real focus, real lens) :
	camera(film, system, transform), mCameraToScreen(projective), mFocus(focus), mLens(lens)
{
	mScreenToRaster = scale(
		vector3(
			static_cast<float>(mFilm->resolution().x),
			static_cast<float>(mFilm->resolution().y),
			static_cast<float>(1)
		));

	mScreenToRaster = mScreenToRaster * scale(
		vector3(
			1.0f / (screen_window.max.x - screen_window.min.x),
			1.0f / (screen_window.min.y - screen_window.max.y),
			1.0f));

	mScreenToRaster = mScreenToRaster * translate(
		glm::vec3(
			-screen_window.min.x,
			-screen_window.max.y,
			0
		));

	mRasterToScreen = mScreenToRaster.inverse();
	mRasterToCamera = mCameraToScreen.inverse() * mRasterToScreen;
}