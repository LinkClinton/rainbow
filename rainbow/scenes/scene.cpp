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

void rainbow::scene::build_accelerator()
{
	// build the accelerator structure
	// if there are some shapes that was referenced too many times
	// we do not put the sub-shapes of shape into accelerator structure because them are too many
	// int this situation, we create a accelerator structure in the shape
	// and put the bounding box of entire shape into accelerator structure
	// to find a sub-shape of shape, we first find the shape in scene's accelerator structure
	// and find the sub-shape in shape's accelerator structure
	// for shape that was referenced only few times
	// we put all sub-shapes into accelerator structure
	constexpr static size_t reference_threshold = 1;
	constexpr static size_t sub_shape_threshold = 1 << 17;
	
	std::unordered_map<std::shared_ptr<shape>, size_t> reference_count;
	
	for (const auto& entity : mEntities) {
		if (!entity->has_component<shape>()) continue;

		reference_count[entity->component<shape>()]++;
	}
	
	std::vector<bounding_box<entity_reference>> boxes;

	for (const auto& entity : mEntities) {
		if (!entity->has_component<shape>()) continue;

		const auto shape = entity->component<shapes::shape>();
		const auto count = reference_count.at(shape);
		
		if (count <= reference_threshold && count * shape->count() <= sub_shape_threshold) {

			for (size_t index = 0; index < shape->count(); index++)
				boxes.push_back(bounding_box<entity_reference>(
					std::make_shared<entity_reference>(entity, index)));
			
		}else {
			// if the reference count greater than "reference_threshold"
			// or the total sub-shapes greater than "sub_shape_threshold"
			// we will put the entire shape into scene's accelerator structure
			// and we will build a accelerator in the shape
			shape->build_accelerator();

			boxes.push_back(bounding_box<entity_reference>(
				std::make_shared<entity_reference>(entity, entity_reference::all)));
		}
		
	}
	
	mAccelerator = std::make_shared<bounding_volume_hierarchy<entity_reference>>(boxes);
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

const std::vector<std::shared_ptr<rainbow::entity>>& rainbow::scene::entities() const noexcept
{
	return mEntities;
}

const std::vector<std::shared_ptr<rainbow::scenes::entity>>& rainbow::scenes::scene::emitters() const noexcept
{
	return mEmitters;
}

const std::vector<std::shared_ptr<rainbow::entity>>& rainbow::scene::environments() const noexcept
{
	return mEnvironments;
}

rainbow::scene::entity_reference::entity_reference(const std::shared_ptr<scenes::entity>& entity, size_t index) :
	entity(entity), index(index)
{
}

std::optional<rainbow::surface_interaction> rainbow::scene::entity_reference::intersect(const ray& ray) const
{
	return index == all ? entity->intersect(ray) : entity->intersect(ray, index);
}

rainbow::bound3 rainbow::scene::entity_reference::bounding_box() const
{
	return index == all ? entity->bounding_box() : entity->bounding_box(index);
}

bool rainbow::scene::entity_reference::visible() const noexcept
{
	return entity->visible();
}
