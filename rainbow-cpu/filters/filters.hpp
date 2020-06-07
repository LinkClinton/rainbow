#pragma once

#include "../../rainbow-core/math/math.hpp"

#include "../interfaces/noncopyable.hpp"

namespace rainbow::cpus::filters {

	using namespace core::math;
	using namespace core;
	
	class filter : public interfaces::noncopyable {
	public:
		explicit filter(const vector2& radius);

		vector2 inverse_radius() const noexcept;

		vector2 radius() const noexcept;

		virtual real evaluate(const vector2& point) const noexcept = 0;
	private:
		vector2 mInverseRadius;
		vector2 mRadius;
	};

}