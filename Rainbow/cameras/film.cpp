#include "film.hpp"

rainbow::cameras::film::film(
	const std::shared_ptr<filters::filter>& filter, 
	const vector2i& resolution,
	const bound2& crop_window) :
	mPixels(static_cast<size_t>(resolution.x) * static_cast<size_t>(resolution.y)),
	mFilter(filter),
	mResolution(resolution)
{
	mPixelsBound.min = vector2i(
		std::ceil(mResolution.x * crop_window.min.x),
		std::ceil(mResolution.y * crop_window.min.y)
	);

	mPixelsBound.max = vector2i(
		std::ceil(mResolution.x * crop_window.max.x),
		std::ceil(mResolution.y * crop_window.max.y)
	);
}
