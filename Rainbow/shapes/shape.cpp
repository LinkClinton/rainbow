#include "shape.hpp"

rainbow::shapes::shape::shape(const std::shared_ptr<rainbow::material>& material, const rainbow::transform& transform) :
	mMaterial(material), mLocalToWorld(transform), mWorldToLocal(transform.inverse())
{
	
}

rainbow::transform rainbow::shapes::shape::transform() const
{
	return mLocalToWorld;
}

std::shared_ptr<rainbow::material> rainbow::shapes::shape::material() const noexcept
{
	return mMaterial;
}
