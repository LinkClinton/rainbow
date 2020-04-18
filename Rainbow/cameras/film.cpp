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

rainbow::cameras::film_tile::film_tile(const bound2i& sample_region, const std::shared_ptr<film>& film) :
	sample_region(sample_region), filter(film->filter())
{
	const auto pixel_bound = film->pixels_bound();
	const auto filter_radius = filter->radius();
	
	filter_region.min = vector2i(
		std::ceil(sample_region.min.x - 0.5 - filter_radius.x),
		std::ceil(sample_region.min.y - 0.5 - filter_radius.y)
	);

	filter_region.max = vector2i(
		std::floor(sample_region.max.x - 0.5 + filter_radius.x) + 1,
		std::floor(sample_region.max.y - 0.5 + filter_radius.y) + 1
	);

	filter_region.min = max(filter_region.min, pixel_bound.min);
	filter_region.max = min(filter_region.max, pixel_bound.max);
	
	pixels = std::vector<pixel>(
		(static_cast<size_t>(filter_region.max.x) - filter_region.min.x) * 
		(static_cast<size_t>(filter_region.max.y) - filter_region.min.y));
}

void rainbow::cameras::film_tile::add_sample(const vector2& position, const spectrum& sample) noexcept
{
	// a sample can contribute some pixels that in a rectangle region with filter's radius.
	// so we need find the rectangle to get the pixels
	// and we will use the filter to compute the spectrum and weight of pixels.
	const auto discrete_position = position - vector2(0.5, 0.5);
	const auto filter_radius = filter->radius();
	
	auto sample_bound = bound2i(
		static_cast<vector2i>(ceil(discrete_position - filter_radius.x)),
		static_cast<vector2i>(floor(discrete_position + filter_radius.y)) + vector2i(1, 1)
	);

	// the sample bound should in the bound of pixels
	sample_bound.min = max(sample_bound.min, filter_region.min);
	sample_bound.max = min(sample_bound.max, filter_region.max);

	const auto filter_region_size = vector2i(
		filter_region.max.x - filter_region.min.x,
		filter_region.max.y - filter_region.min.y
	);
	
	// compute the sum of spectrum and the filter weight
	// formula : sum of (f(x - xi, y - yi) * w(xi, yi) * L(xi, yi)) / sum of (f(x - xi, y - yi))
	// f is the value of filter function, w is the weight of sample(in this version, it is 1)
	for (auto y = sample_bound.min.y; y < sample_bound.max.y; y++) {
		for (auto x = sample_bound.min.x; x < sample_bound.max.x; x++) {
			const auto filter_value = filter->evaluate(
				vector2(x - discrete_position.x, y - discrete_position.y));

			const auto pixel_index = (y - filter_region.min.y) * filter_region_size.x + (x - filter_region.min.x);
			
			pixels[pixel_index].add_sample(sample * filter_value, filter_value);
		}
	}
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
	const auto to_byte = [](real value)
	{
		return static_cast<byte>(clamp(
			255 * gamma_correct(value) + 0.5f,
			static_cast<real>(0),
			static_cast<real>(255)));
	};
	
	auto colors = std::vector<byte>(image_size * 4, 255);

	for (size_t index = 0; index < image_size; index++) {
		const auto x_position = index % mResolution.x;
		const auto y_position = index / mResolution.x;

		if (x_position < mPixelsBound.min.x || x_position >= mPixelsBound.max.x) continue;
		if (y_position < mPixelsBound.min.y || y_position >= mPixelsBound.max.y) continue;
		
		const auto spectrum = mPixels[index].spectrum();
		
		colors[index * 4 + 0] = to_byte(spectrum.red());
		colors[index * 4 + 1] = to_byte(spectrum.green());
		colors[index * 4 + 2] = to_byte(spectrum.blue());
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

void rainbow::cameras::film::add_tile(const film_tile& tile)
{
	const auto filter_region_size = vector2i(
		tile.filter_region.max.x - tile.filter_region.min.x,
		tile.filter_region.max.y - tile.filter_region.min.y
	);
	
	for (auto y = tile.filter_region.min.y; y < tile.filter_region.max.y; y++) {
		for (auto x = tile.filter_region.min.x; x < tile.filter_region.max.x; x++) {
			const auto tile_pixel_index = 
				(y - tile.filter_region.min.y) * filter_region_size.x + (x - tile.filter_region.min.x);
			const auto& tile_pixel = tile.pixels[tile_pixel_index];
			
			mPixels[pixel_index(vector2i(x, y))].add_sample(tile_pixel.spectrum_sum, tile_pixel.filter_weight);
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

std::shared_ptr<rainbow::filter> rainbow::cameras::film::filter() const noexcept
{
	return mFilter;
}

rainbow::int32 rainbow::cameras::film::pixel_index(const vector2i& position) const noexcept
{
	return position.y * mResolution.x + position.x;
}

rainbow::real rainbow::cameras::gamma_correct(real value)
{
	if (value <= 0.0031308f) return 12.92f * value;

	return 1.055f * pow(value, 1.f / 2.4f) - 0.055f;
}
