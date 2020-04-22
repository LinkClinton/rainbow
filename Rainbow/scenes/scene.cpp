#include "scene.hpp"

void rainbow::scenes::scene::add_entity(const std::shared_ptr<entity>& entity)
{
	if (entity->has_component<emitter>()) mEmitters.push_back(entity);

	mEntities.push_back(entity);
}

std::optional<rainbow::surface_interaction> rainbow::scenes::scene::intersect(const ray& ray) const
{
	std::optional<surface_interaction> nearest_interaction;

	for (const auto& entity : mEntities) {
		const auto interaction = entity->intersect(ray);

		if (interaction.has_value()) nearest_interaction = interaction;
	}

	return nearest_interaction;
}

std::optional<rainbow::surface_interaction> rainbow::scenes::scene::intersect_with_shadow_ray(const ray& ray) const
{
	return intersect(ray);
}

const std::vector<std::shared_ptr<rainbow::scenes::entity>>& rainbow::scenes::scene::emitters() const noexcept
{
	return mEmitters;
}
