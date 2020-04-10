#include "scene.hpp"

void rainbow::scenes::scene::add_shape(const std::shared_ptr<shape>& shape)
{
	mShapes.push_back(shape);
}

void rainbow::scenes::scene::add_light(const std::shared_ptr<light>& light)
{
	mLights.push_back(light);
}

rainbow::surface_interaction rainbow::scenes::scene::intersect(const ray& ray) const
{
	surface_interaction interaction;

	for (const auto& shape : mShapes)
		interaction = shape->intersect(ray);

	return interaction;
}
