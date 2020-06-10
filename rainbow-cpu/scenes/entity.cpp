#include "entity.hpp"

using namespace rainbow::cpus::shared::interactions;
using namespace rainbow::cpus::shared::spectrums;

rainbow::cpus::scenes::entity::entity(
	const std::shared_ptr<material>& material, 
	const std::shared_ptr<emitter>& emitter,
	const std::shared_ptr<shape>& shape,
	const std::shared_ptr<media>& media,
	const shared::transform& transform) :
	mMaterial(material), mEmitter(emitter), mShape(shape), mMedia(media),
	mLocalToWorld(transform), mWorldToLocal(transform.inverse())
{
}

std::optional<surface_interaction> rainbow::cpus::scenes::entity::intersect(const ray& ray, size_t index) const
{
	// we do not start intersect test when the entity does not have shape
	if (!has_component<shape>()) return std::nullopt;

	// transform ray from world to local
	const auto local_ray = mWorldToLocal(ray);
	auto interaction = mShape->intersect(local_ray, index);

	// if the ray does not intersect the shape, we return std::nullopt means no intersect
	if (!interaction.has_value()) return std::nullopt;

	interaction->entity = shared_from_this();

	// if the transform has scale transform, the length of should be scale too.
	ray.length = mLocalToWorld(local_ray).length;

	// transform it from local to world space.
	return mLocalToWorld(interaction.value());
}

std::optional<surface_interaction> rainbow::cpus::scenes::entity::intersect(const ray& ray) const
{
	// we do not start intersect test when the entity does not have shape
	if (!has_component<shape>()) return std::nullopt;

	// transform ray from world to local
	const auto local_ray = mWorldToLocal(ray);
	auto interaction = mShape->intersect(local_ray);

	// if the ray does not intersect the shape, we return std::nullopt means no intersect
	if (!interaction.has_value()) return std::nullopt;
	
	interaction->entity = shared_from_this();
	
	// if the transform has scale transform, the length of should be scale too.
	ray.length = mLocalToWorld(local_ray).length;

	// transform it from local to world space.
	return mLocalToWorld(interaction.value());
}

rainbow::core::math::bound3 rainbow::cpus::scenes::entity::bounding_box(size_t index) const
{
	assert(mShape != nullptr);

	// we will create the bounding box(axis-aligned-bounding-box) in world space
	// because if we create it on local space and transform it to world space may increase the volume of box
	// it will return the bounding box of one part of shape
	return mShape->bounding_box(mLocalToWorld, index);
}

rainbow::core::math::bound3 rainbow::cpus::scenes::entity::bounding_box() const
{
	assert(mShape != nullptr);

	// we will create the bounding box(axis-aligned-bounding-box) in world space
	// because if we create it on local space and transform it to world space may increase the volume of box
	// it will return the bounding box of all part of shape
	return mShape->bounding_box(mLocalToWorld);
}

bool rainbow::cpus::scenes::entity::visible() const noexcept
{
	return has_component<shape>() && has_component<material>();
}

spectrum rainbow::cpus::scenes::entity::power() const noexcept
{
	return mEmitter->power(mShape);
}

transform entity::transform() const noexcept
{
	return mLocalToWorld;
}
