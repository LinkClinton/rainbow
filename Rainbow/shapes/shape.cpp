#include "shape.hpp"

rainbow::shapes::shape::shape(const rainbow::transform& transform) :
	mLocalToWorld(transform), mWorldToLocal(transform.inverse())
{
	
}

rainbow::transform rainbow::shapes::shape::transform() const
{
	return mLocalToWorld;
}
