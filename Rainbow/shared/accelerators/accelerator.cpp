#include "accelerator.hpp"

rainbow::accelerators::bounding_box::bounding_box(const std::shared_ptr<const scenes::entity>& entity, size_t index) :
	entity(entity), box(entity->bounding_box(index)), index(index)
{
}

rainbow::accelerators::accelerator::accelerator(const std::vector<bounding_box>& boxes) : mBoundingBoxes(boxes)
{
}
