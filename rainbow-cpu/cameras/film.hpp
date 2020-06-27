#pragma once

#include "../../rainbow-core/math/math.hpp"

#include "../shared/spectrums/spectrum.hpp"
#include "../interfaces/noncopyable.hpp"
#include "../filters/filters.hpp"

#include <vector>
#include <memory>
#include <string>

namespace rainbow::cpus::cameras {

	using namespace shared::spectrums;
	using namespace filters;
	using namespace math;

	class film;

	struct pixel {
		spectrum spectrum_sum;
		real filter_weight;

		pixel();

		pixel(const spectrum& spectrum, const real weight);

		void add_sample(const spectrum& spectrum, const real weight) noexcept;

		spectrum spectrum() const;
	};

	struct film_tile {
		bound2i sample_region;
		bound2i filter_region;

		std::vector<pixel> pixels;

		std::shared_ptr<filter> filter;

		film_tile() = default;

		film_tile(
			const bound2i& sample_region,
			const std::shared_ptr<film>& film);

		void add_sample(const vector2& position, const spectrum& sample) noexcept;
	};

	class film final : public interfaces::noncopyable {
	public:
		explicit film(
			const std::shared_ptr<filter>& filter,
			const vector2i& resolution,
			const bound2& crop_window,
			real scale = 1);

		void write(const std::string& file_name) const noexcept;

		void add_sample(const vector2& position, const spectrum& sample) noexcept;

		void set_pixel(const vector2i& position, const spectrum& value);

		void set_pixel(size_t index, const spectrum& value);
		
		void add_tile(const film_tile& tile);

		vector2i resolution() const noexcept;

		bound2i pixels_bound() const noexcept;

		std::shared_ptr<filter> filter() const noexcept;
	private:
		int32 pixel_index(const vector2i& position) const noexcept;
	private:
		std::vector<cameras::pixel> mPixels;

		std::shared_ptr<filters::filter> mFilter;

		vector2i mResolution;
		bound2i mPixelsBound;

		real mScale = 1;
	};

}