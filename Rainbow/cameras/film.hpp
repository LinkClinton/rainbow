#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../filters/filters.hpp"

#include "../shared/spectrums/spectrum.hpp"
#include "../shared/math/math.hpp"

#include <vector>
#include <memory>

#define __STB_WRITE_IMAGE__

namespace rainbow {

	using namespace spectrums;
	using namespace filters;
	using namespace math;
	
	namespace cameras {

		struct pixel {
			spectrum spectrum_sum;
			real filter_weight;

			pixel();

			pixel(const spectrum& spectrum, const real weight);

			void add_sample(const spectrum& spectrum, const real weight) noexcept;

			spectrum spectrum() const;
		};
		
		class film final : public interfaces::noncopyable {
		public:
			explicit film(
				const std::shared_ptr<filter>& filter,
				const vector2i& resolution,
				const bound2& crop_window);

#ifdef __STB_WRITE_IMAGE__
			void write(const std::string& file_name) const noexcept;
#endif
			
			void add_sample(const vector2& position, const spectrum& sample) noexcept;
	
			vector2i resolution() const noexcept;

			bound2i pixels_bound() const noexcept;
		private:
			int32 pixel_index(const vector2i& position) const noexcept;
		private:
			std::vector<cameras::pixel> mPixels;
			
			std::shared_ptr<filter> mFilter;

			vector2i mResolution;
			bound2i mPixelsBound;
		};

		real gamma_correct(real value);
		
	}
}