#include "scene.hpp"

#include "../shared/accelerators/bounding_volume_hierarchy.hpp"
#include "../shared/logs/log.hpp"

void rainbow::scenes::scene::add_entity(const std::shared_ptr<entity>& entity)
{
	if (entity->has_component<emitter>()) {
		mEmitters.push_back(entity);

		if (entity->component<emitter>()->is_environment())
			mEnvironments.push_back(entity);
	}
	
	mEntities.push_back(entity);
}

void rainbow::scene::build_emitters_distribution()
{
	if (mEmitters.empty()) return;

	std::vector<real> emitters_power;

	for (const auto& emitter : mEmitters) 
		emitters_power.push_back(emitter->power().luminance());

	mEmittersDistribution = std::make_shared<distribution1d>(emitters_power);
}

void rainbow::scene::build_accelerator()
{
	std::vector<bounding_box> boxes;

	for (const auto& entity : mEntities) {
		if (!entity->has_component<shape>()) continue;

		for (size_t index = 0; index < entity->component<shape>()->count(); index++) 
			boxes.push_back(bounding_box(entity, index));
	}
	
	mAccelerator = std::make_shared<bounding_volume_hierarchy>(boxes);
}

std::optional<rainbow::surface_interaction> rainbow::scenes::scene::intersect(const ray& ray) const
{
	if (mAccelerator != nullptr) return mAccelerator->intersect(ray);
	
	std::optional<surface_interaction> nearest_interaction;

	for (const auto& entity : mEntities) {
		const auto interaction = entity->intersect(ray);

		if (interaction.has_value()) nearest_interaction = interaction;
	}

	return nearest_interaction;
}

std::optional<rainbow::surface_interaction> rainbow::scenes::scene::intersect_with_shadow_ray(const ray& ray) const
{
	if (mAccelerator != nullptr) return mAccelerator->intersect_with_shadow_ray(ray);
	
	std::optional<surface_interaction> nearest_interaction;

	for (const auto& entity : mEntities) {
		if (!entity->visible()) continue;
		
		const auto interaction = entity->intersect(ray);

		if (interaction.has_value()) nearest_interaction = interaction;
	}

	return nearest_interaction;
}

const std::vector<std::shared_ptr<rainbow::scenes::entity>>& rainbow::scenes::scene::emitters() const noexcept
{
	return mEmitters;
}

const std::vector<std::shared_ptr<rainbow::entity>>& rainbow::scene::environments() const noexcept
{
	return mEnvironments;
}

std::shared_ptr<rainbow::distribution1d> rainbow::scene::emitters_distribution() const noexcept
{
	return mEmittersDistribution;
}
