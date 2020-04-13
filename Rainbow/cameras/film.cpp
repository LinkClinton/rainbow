#include "film.hpp"

#ifdef __STB_WRITE_IMAGE__

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>

#endif

rainbow::cameras::pixel::pixel() : pixel(spectrums::spectrum(0), 0)
{
	
}

void rainbow::cameras::pixel::add_sample(const spectrums::spectrum& spectrum, const real weight) noexcept
{
	spectrum_sum += spectrum;
	filter_weight += weight;
}

rainbow::spectrum rainbow::cameras::pixel::spectrum() const
{
	assert(filter_weight != 0);

	return spectrum_sum / filter_weight;
}

rainbow::cameras::pixel::pixel(const spectrums::spectrum& spectrum, const real weight) :
	spectrum_sum(spectrum), filter_weight(weight)
{
	
}

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

#ifdef __STB_WRITE_IMAGE__

void rainbow::cameras::film::write(const std::string& file_name) const noexcept
{
	using byte = unsigned char;
	
	const auto image_size = static_cast<size_t>(mResolution.x) * static_cast<size_t>(mResolution.y);

	auto colors = std::vector<byte>(image_size * 4, 255);

	for (size_t index = 0; index < image_size; index++) {
		const auto x_position = index % mResolution.x;
		const auto y_position = index / mResolution.x;

		if (x_position < mPixelsBound.min.x || x_position >= mPixelsBound.max.x) continue;
		if (y_position < mPixelsBound.min.y || y_position >= mPixelsBound.max.y) continue;
		
		const auto spectrum = mPixels[index].spectrum();
		colors[index * 4 + 0] = static_cast<byte>(spectrum.red() * 255);
		colors[index * 4 + 1] = static_cast<byte>(spectrum.green() * 255);
		colors[index * 4 + 2] = static_cast<byte>(spectrum.blue() * 255);
		colors[index * 4 + 3] = 255;
	}

	stbi_write_png((file_name + ".png").c_str(),
		mResolution.x,
		mResolution.y,
		4,
		colors.data(),
		0);
}

#endif

void rainbow::cameras::film::add_sample(const vector2& position, const spectrum& sample) noexcept
{
	// a sample can contribute some pixels that in a rectangle region with filter's radius.
	// so we need find the rectangle to get the pixels
	// and we will use the filter to compute the spectrum and weight of pixels.
	const auto discrete_position = position - vector2(0.5, 0.5);

	auto sample_bound = bound2i(
		static_cast<vector2i>(ceil(discrete_position - mFilter->radius())),
		static_cast<vector2i>(floor(discrete_position + mFilter->radius())) + vector2i(1, 1)
	);

	// the sample bound should in the bound of pixels
	sample_bound.min = max(sample_bound.min, mPixelsBound.min);
	sample_bound.max = min(sample_bound.max, mPixelsBound.max);

	// compute the sum of spectrum and the filter weight
	// formula : sum of (f(x - xi, y - yi) * w(xi, yi) * L(xi, yi)) / sum of (f(x - xi, y - yi))
	// f is the value of filter function, w is the weight of sample(in this version, it is 1)
	for (auto y = sample_bound.min.y; y < sample_bound.max.y; y++) {
		for (auto x = sample_bound.min.x; x < sample_bound.max.x; x++) {
			const auto filter_value = mFilter->evaluate(
				vector2(x - discrete_position.x, y - discrete_position.y));

			auto& pixel = mPixels[pixel_index(vector2i(x, y))];
			
			pixel.add_sample(sample * filter_value, filter_value);
		}
	}
}

rainbow::vector2i rainbow::cameras::film::resolution() const noexcept
{
	return mResolution;
}

rainbow::bound2i rainbow::cameras::film::pixels_bound() const noexcept
{
	return mPixelsBound;
}

rainbow::int32 rainbow::cameras::film::pixel_index(const vector2i& position) const noexcept
{
	return position.y * mResolution.x + position.x;
}
