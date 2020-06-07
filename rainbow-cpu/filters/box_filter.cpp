#include "box_filter.hpp"

rainbow::cpus::filters::box_filter::box_filter(const vector2& radius) : filter(radius)
{
}

rainbow::core::real rainbow::cpus::filters::box_filter::evaluate(const vector2& point) const noexcept
{
	return 1;
}
