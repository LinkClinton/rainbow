#include "box_filter.hpp"

rainbow::filters::box_filter::box_filter(const vector2& radius) : filter(radius)
{
}

rainbow::real rainbow::filters::box_filter::evaluate(const vector2& point) const noexcept
{
	return 1;
}
