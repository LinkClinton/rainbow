#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../filters/filters.hpp"

#include "../shared/math/math.hpp"

#include <vector>
#include <memory>

namespace rainbow {

	using namespace filters;
	using namespace math;
	
	namespace cameras {

		struct pixel {
			
		};
		
		class film final : public interfaces::noncopyable {
		public:
			explicit film(
				const std::shared_ptr<filters::filter>& filter,
				const vector2i& resolution,
				const bound2& crop_window);
		private:
			std::vector<pixel> mPixels;
			
			std::shared_ptr<filter> mFilter;

			vector2i mResolution;
			bound2i mPixelsBound;
		};
		
	}
}