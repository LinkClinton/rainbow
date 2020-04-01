#include "filters.hpp"

rainbow::filters::filter::filter(const vector2& radius) :
	mRadius(radius), mInverseRadius(static_cast<real>(1) / radius)
{
	
}

rainbow::vector2 rainbow::filters::filter::inverse_radius() const noexcept
{
	return mInverseRadius;
}

rainbow::vector2 rainbow::filters::filter::radius() const noexcept
{
	return mRadius;
}
