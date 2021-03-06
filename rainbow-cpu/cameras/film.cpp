#include "film.hpp"

#include "../../rainbow-core/atomic_function.hpp"
#include "../../rainbow-core/file_system.hpp"
#include "../../rainbow-core/logs/log.hpp"

using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::cameras::pixel::pixel() : pixel(shared::spectrums::spectrum(0), 0)
{
	
}

void rainbow::cpus::cameras::pixel::add_sample(const shared::spectrums::spectrum& spectrum, const real weight) noexcept
{
	spectrum_sum += spectrum;
	filter_weight += weight;
}

spectrum rainbow::cpus::cameras::pixel::spectrum() const
{
	assert(filter_weight != 0);

	return spectrum_sum / filter_weight;
}

rainbow::cpus::cameras::pixel::pixel(const shared::spectrums::spectrum& spectrum, const real weight) :
	spectrum_sum(spectrum), filter_weight(weight)
{
	
}

rainbow::cpus::cameras::film_tile::film_tile(const bound2i& sample_region, const std::shared_ptr<film>& film) :
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

void rainbow::cpus::cameras::film_tile::add_sample(const vector2& position, const spectrum& sample) noexcept
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

rainbow::cpus::cameras::film::film(
	const std::shared_ptr<filters::filter>& filter,
	const vector2i& resolution,
	const bound2& crop_window,
	real scale) :
	mValues(static_cast<size_t>(resolution.x)* static_cast<size_t>(resolution.y)),
	mPixels(static_cast<size_t>(resolution.x)* static_cast<size_t>(resolution.y)),
	mFilter(filter), mResolution(resolution), mScale(scale)
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

void rainbow::cpus::cameras::film::write(const std::string& file_name) const noexcept
{
	using byte = unsigned char;
	
	const auto image_size = static_cast<size_t>(mResolution.x) * static_cast<size_t>(mResolution.y);
	const auto to_byte = [](real value)
	{
		return static_cast<byte>(math::clamp(
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
		
		if (spectrum.has_nan()) logs::warn("pixel [{0}, {1}] has nan.", x_position, y_position);

		std::array<real, 3> values = {
			spectrum[0] + mValues[index][0],
			spectrum[1] + mValues[index][1],
			spectrum[2] + mValues[index][2]
		};
		
		colors[index * 4 + 0] = to_byte(values[0] * mScale);
		colors[index * 4 + 1] = to_byte(values[1] * mScale);
		colors[index * 4 + 2] = to_byte(values[2] * mScale);
		colors[index * 4 + 3] = 255;
	}

	file_system::write_image(file_name + ".png", colors, mResolution.x, mResolution.y);
}

void rainbow::cpus::cameras::film::add_sample(const vector2& position, const spectrum& sample) noexcept
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

void rainbow::cpus::cameras::film::add_pixel(const vector2i& position, const spectrum& value) noexcept
{
	if (position.x >= mPixelsBound.max.x || position.x < mPixelsBound.min.x) return;
	if (position.y >= mPixelsBound.max.y || position.y < mPixelsBound.min.y) return;

	const auto index = pixel_index(position);

	for (size_t channel = 0; channel < 3; channel++)
		atomic_real_add(mValues[index][channel], value[channel]);
}

void rainbow::cpus::cameras::film::set_pixel(const vector2i& position, const spectrum& value)
{
	mPixels[pixel_index(position)] = pixel(value, 1);
}

void rainbow::cpus::cameras::film::set_pixel(size_t index, const spectrum& value)
{
	mPixels[index] = pixel(value, 1);
}

void rainbow::cpus::cameras::film::add_tile(const film_tile& tile)
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

rainbow::core::math::vector2i rainbow::cpus::cameras::film::resolution() const noexcept
{
	return mResolution;
}

rainbow::core::math::bound2i rainbow::cpus::cameras::film::pixels_bound() const noexcept
{
	return mPixelsBound;
}

std::shared_ptr<rainbow::cpus::filters::filter> rainbow::cpus::cameras::film::filter() const noexcept
{
	return mFilter;
}

rainbow::core::int32 rainbow::cpus::cameras::film::pixel_index(const vector2i& position) const noexcept
{
	return position.y * mResolution.x + position.x;
}