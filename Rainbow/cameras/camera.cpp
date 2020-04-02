#include "camera.hpp"

rainbow::cameras::camera::camera(const std::shared_ptr<film>& film, const transform& transform) :
	mFilm(film), mCameraToWorld(transform)
{
	
}

rainbow::cameras::projective_camera::projective_camera(
	const std::shared_ptr<film>& film, 
	const transform& projective,
	const transform& transform, 
	const bound2& screen_window) :
	camera(film, transform), mCameraToScreen(projective)
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
