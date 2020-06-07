#pragma once

#include "filters.hpp"

namespace rainbow::cpus::filters {

	class gaussian_filter final : public filter {
	public:
		explicit gaussian_filter(const vector2& radius = vector2(2), real alpha = 2);

		~gaussian_filter() = default;

		real evaluate(const vector2& point) const noexcept override;
	private:
		real gaussian(real d, real exp) const noexcept;

		real mAlpha;

		real mExpX;
		real mExpY;
	};
	
}
