#pragma once

#include "shapes.hpp"

rainbow::shapes::shape::shape(const rainbow::transform& transform) : mTransform(transform)
{
	
}

rainbow::transform rainbow::shapes::shape::transform() const
{
	return mTransform;
}
