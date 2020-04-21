#include "scene.hpp"

void rainbow::scenes::scene::add_shape(const std::shared_ptr<shape>& shape)
{
	mShapes.push_back(shape);
}

void rainbow::scenes::scene::add_emitter(const std::shared_ptr<emitter>& emitter)
{
	mEmitters.push_back(emitter);
}

std::optional<rainbow::surface_interaction> rainbow::scenes::scene::intersect(const ray& ray) const
{
	std::optional<surface_interaction> nearest_interaction;

	for (const auto& shape : mShapes) {
		const auto interaction = shape->intersect(ray);

		if (interaction.has_value()) nearest_interaction = interaction;
	}

	return nearest_interaction;
}

std::optional<rainbow::surface_interaction> rainbow::scenes::scene::intersect_with_shadow_ray(const ray& ray) const
{
	return intersect(ray);
}

const std::vector<std::shared_ptr<rainbow::emitter>>& rainbow::scenes::scene::emitters() const noexcept
{
	return mEmitters;
}

const std::vector<std::shared_ptr<rainbow::shape>>& rainbow::scenes::scene::shapes() const noexcept
{
	return mShapes;
}
