#pragma once

#include "filters.hpp"

namespace rainbow {

	namespace filters {

		class box_filter final : public filter {
		public:
			explicit box_filter(const vector2& radius);

			real evaluate(const vector2& point) const noexcept override;
		};
		
	}
}
