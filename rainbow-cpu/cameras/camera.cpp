#include "camera.hpp"

rainbow::cpus::cameras::camera_sample::camera_sample(const vector2& position, const vector2& lens) :
	position(position), lens(lens)
{
}

rainbow::cpus::cameras::camera::camera(const std::shared_ptr<cameras::film>& film, const transform& transform) :
	mFilm(film), mCameraToWorld(transform)
{
	
}

std::shared_ptr<rainbow::cpus::cameras::film> rainbow::cpus::cameras::camera::film() const noexcept
{
	return mFilm;
}

rainbow::cpus::cameras::projective_camera::projective_camera(
	const std::shared_ptr<cameras::film>& film, 
	const transform& projective,
	const transform& transform, 
	const bound2& screen_window,
	real focus, real lens) :
	camera(film, transform), mCameraToScreen(projective), mFocus(focus), mLens(lens)
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