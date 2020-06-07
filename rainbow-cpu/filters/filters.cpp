#include "filters.hpp"

rainbow::cpus::filters::filter::filter(const vector2& radius) :
	mInverseRadius(static_cast<real>(1) / radius), mRadius(radius)
{
	
}

rainbow::core::math::vector2 rainbow::cpus::filters::filter::inverse_radius() const noexcept
{
	return mInverseRadius;
}

rainbow::core::math::vector2 rainbow::cpus::filters::filter::radius() const noexcept
{
	return mRadius;
}
