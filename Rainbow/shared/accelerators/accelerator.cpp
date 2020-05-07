#include "accelerator.hpp"

rainbow::accelerators::bounding_box::bounding_box(const std::shared_ptr<const scenes::entity>& entity, size_t index) :
	entity(entity), box(entity->bounding_box(index)), index(index)
{
}

rainbow::accelerators::bounding_box::bounding_box(const vector3& v0, const vector3& v1) :
	box(v0, v1)
{
}

rainbow::accelerators::bounding_box::bounding_box(const bounding_box& box0, const bounding_box& box1)
{
	box.min = min(box0.box.min, box1.box.min);
	box.max = max(box0.box.max, box1.box.max);
}

bool rainbow::accelerators::bounding_box::intersect(const ray& ray) const
{
	auto t0 = static_cast<real>(0), t1 = ray.length;

	// enum the slab of axis-aligned bounding box
	for (int dimension = 0; dimension < 3; dimension++) {
		// if the direction[dimension] is 0, the inv_direction will be INF.
		const auto inv_direction = 1 / ray.direction[dimension];

		// find the t value of intersect point on the ray
		auto near = (box.min[dimension] - ray.origin[dimension]) * inv_direction;
		auto far = (box.max[dimension] - ray.origin[dimension]) * inv_direction;

		// if the direction of ray is negative, the near will greater than far, we need swap them.
		if (near > far) std::swap(near, far);

		t0 = max(t0, near);
		t1 = min(t1, far);

		if (t0 > t1) return false;
	}

	return true;
}

void rainbow::accelerators::bounding_box::union_it(const bounding_box& box) noexcept
{
	this->box.min = min(this->box.min, box.box.min);
	this->box.max = max(this->box.max, box.box.max);
}

void rainbow::accelerators::bounding_box::union_it(const vector3& point) noexcept
{
	box.min = min(box.min, point);
	box.max = max(box.max, point);
}

size_t rainbow::accelerators::bounding_box::max_dimension() const noexcept
{
	const auto length = box.max - box.min;

	if (length.x > length.y && length.x > length.z) return 0;
	if (length.y > length.z) return 1;
	return 2;
}

rainbow::vector3 rainbow::accelerators::bounding_box::centroid() const noexcept
{
	return (box.min + box.max) * static_cast<real>(0.5);
}

rainbow::accelerators::accelerator::accelerator(const std::vector<bounding_box>& boxes) : mBoundingBoxes(boxes)
{
}
