#include "gaussian_filter.hpp"

rainbow::filters::gaussian_filter::gaussian_filter(const vector2& radius, real alpha) :
	filter(radius), mAlpha(alpha),
	mExpX(exp(-alpha * radius.x * radius.x)),
	mExpY(exp(-alpha * radius.y * radius.y))
{
}

rainbow::real rainbow::filters::gaussian_filter::evaluate(const vector2& point) const noexcept
{
	return gaussian(point.x, mExpX) * gaussian(point.y, mExpY);
}

rainbow::real rainbow::filters::gaussian_filter::gaussian(real d, real exp) const noexcept
{
	return max(static_cast<real>(0), math::exp(-mAlpha * d * d) - exp);
}
