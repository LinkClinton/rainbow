#pragma once

#include "filters.hpp"

namespace rainbow::cpus::filters {

	class box_filter final : public filter {
	public:
		explicit box_filter(const vector2& radius = vector2(0.5));

		real evaluate(const vector2& point) const noexcept override;
	};

}
