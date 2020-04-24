#include "entity.hpp"

rainbow::scenes::entity::entity(
	const std::shared_ptr<material>& material, 
	const std::shared_ptr<emitter>& emitter,
	const std::shared_ptr<shape>& shape, 
	const transform& transform) :
	mMaterial(material), mEmitter(emitter), mShape(shape),
	mLocalToWorld(transform), mWorldToLocal(transform.inverse())
{
}

std::optional<rainbow::surface_interaction> rainbow::scenes::entity::intersect(const ray& ray) const
{
	// we do not start intersect test when the entity does not have shape
	if (!has_component<shape>()) return std::nullopt;

	// transform ray from world to local
	const auto local_ray = mWorldToLocal(ray);
	auto interaction = mShape->intersect(local_ray);

	interaction->entity = shared_from_this();
	
	ray.length = local_ray.length;
	
	// if the ray does not intersect the shape, we return std::nullopt means no intersect
	if (!interaction.has_value()) return std::nullopt;

	// transform it from local to world space.
	return mLocalToWorld(interaction.value());
}

bool rainbow::entity::visible() const noexcept
{
	return has_component<shape>() && has_component<material>();
}