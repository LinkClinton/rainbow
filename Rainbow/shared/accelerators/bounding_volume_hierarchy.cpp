#include "bounding_volume_hierarchy.hpp"

#include <algorithm>
#include <stack>

#define BOUNDING_VOLUME_HIERARCHY_POOL_SIZE 1024 * 1024

rainbow::accelerators::bounding_volume_hierarchy_node::bounding_volume_hierarchy_node(
	const accelerators::bounding_box& box, size_t begin, size_t end) :
	box(box), begin(begin), end(end)
{
}

rainbow::accelerators::bounding_volume_hierarchy_node::bounding_volume_hierarchy_node(
	bounding_volume_hierarchy_node* left, bounding_volume_hierarchy_node* right, size_t begin, size_t end, size_t axis) :
	right(right), left(left), box(left->box, right->box), begin(begin), end(end), axis(axis)
{
}

bool rainbow::accelerators::bounding_volume_hierarchy_node::is_leaf() const noexcept
{
	return left == nullptr && right == nullptr;
}

rainbow::accelerators::bounding_volume_hierarchy_allocator::bounding_volume_hierarchy_allocator()
{
	mMemoryPools.push_back(std::vector<bounding_volume_hierarchy_node>(BOUNDING_VOLUME_HIERARCHY_POOL_SIZE));
}

rainbow::accelerators::bounding_volume_hierarchy_node* rainbow::accelerators::bounding_volume_hierarchy_allocator::allocate()
{
	// when the nodes greater than the poll size, we will create a new pool to allocate the memory
	// we do not need a full consecutive pool of nodes
	if (mCurrentNodes == mMemoryPools.back().size()) {
		mMemoryPools.push_back(std::vector<bounding_volume_hierarchy_node>(BOUNDING_VOLUME_HIERARCHY_POOL_SIZE));

		mCurrentNodes = 0;
	}

	return &mMemoryPools.back()[mCurrentNodes++];
}

rainbow::accelerators::bounding_volume_hierarchy::bounding_volume_hierarchy(const std::vector<bounding_box>& boxes) :
	accelerator(boxes), mRoot(recursive_build(0, boxes.size()))
{
}

std::optional<rainbow::surface_interaction> rainbow::accelerators::bounding_volume_hierarchy::intersect(
	const ray& ray) const
{
	std::stack<bounding_volume_hierarchy_node*> stack;
	std::optional<surface_interaction> nearest_interaction;
	
	stack.push(mRoot);

	const auto inv_direction = static_cast<real>(1) / ray.direction;
	const bool is_negative_direction[3] = {
		inv_direction.x < 0,
		inv_direction.y < 0,
		inv_direction.z < 0
	};
	
	while (!stack.empty()) {
		const auto node = stack.top(); stack.pop();

		// if the ray is not intersect with this node
		// there are no entities in this node's bounding box intersect with ray
		if (!node->box.intersect(ray)) continue;

		// if the node is leaf, we can test the entities in this node with ray
		if (node->is_leaf()) {

			// loop all entities in this node to find the nearest interaction
			for (auto index = node->begin; index < node->end; index++) {
				const auto interaction = 
					mBoundingBoxes[index].entity->intersect(ray, mBoundingBoxes[index].index);

				if (interaction.has_value()) nearest_interaction = interaction;
			}

			continue;
		}

		// if the direction of ray is negative, the near child should be the right child
		// if the direction of ray is not negative, the near child should be the left child
		// we will travel the near child firstly
		if (is_negative_direction[node->axis])
			stack.push(node->left), stack.push(node->right);
		else
			stack.push(node->right), stack.push(node->left);
	}

	return nearest_interaction;
}

std::optional<rainbow::surface_interaction> rainbow::accelerators::bounding_volume_hierarchy::intersect_with_shadow_ray(
	const ray& ray) const
{
	std::stack<bounding_volume_hierarchy_node*> stack;
	std::optional<surface_interaction> nearest_interaction;

	stack.push(mRoot);

	const auto inv_direction = static_cast<real>(1) / ray.direction;
	const bool is_negative_direction[3] = {
		inv_direction.x < 0,
		inv_direction.y < 0,
		inv_direction.z < 0
	};

	while (!stack.empty()) {
		const auto node = stack.top(); stack.pop();

		// if the ray is not intersect with this node
		// there are no entities in this node's bounding box intersect with ray
		if (!node->box.intersect(ray)) continue;

		// if the node is leaf, we can test the entities in this node with ray
		if (node->is_leaf()) {

			// loop all entities in this node to find the nearest interaction
			for (auto index = node->begin; index < node->end; index++) {
				// because we will intersect with shadow ray, so we do not intersect invisible entity
				if (!mBoundingBoxes[index].entity->visible()) continue;
				
				const auto interaction = mBoundingBoxes[index].entity->intersect(ray);

				if (interaction.has_value()) nearest_interaction = interaction;
			}

			continue;
		}

		// if the direction of ray is negative, the near child should be the right child
		// if the direction of ray is not negative, the near child should be the left child
		// we will travel the near child firstly
		if (is_negative_direction[node->axis])
			stack.push(node->left), stack.push(node->right);
		else
			stack.push(node->right), stack.push(node->left);
	}

	return nearest_interaction;
}

rainbow::accelerators::bounding_volume_hierarchy_node* rainbow::accelerators::bounding_volume_hierarchy::recursive_build(size_t begin, size_t end)
{
	const auto node = mAllocator.allocate();

	auto union_box = mBoundingBoxes[begin];

	// union all boxes in these node, this box shows the space of this node
	for (auto index = begin + 1; index < end; index++)
		union_box.union_it(mBoundingBoxes[index]);

	// if there is only one entity in this node, the node should be leaf
	if (end - begin == 1) return &((*node) = bounding_volume_hierarchy_node(union_box, begin, end));

	auto centroid_box = bounding_box(
		mBoundingBoxes[begin].centroid(),
		mBoundingBoxes[begin].centroid()
	);

	// union centroid of all boxes to create ta centroid_box
	for (auto index = begin + 1; index < end; index++)
		centroid_box.union_it(mBoundingBoxes[index].centroid());

	const auto dimension = centroid_box.max_dimension();

	// if the centroid of all boxes are same, we can not divide them into nodes,
	// so the node should be leaf
	if (centroid_box.box.max[static_cast<int>(dimension)] == centroid_box.box.min[static_cast<int>(dimension)])
		return &((*node) = bounding_volume_hierarchy_node(union_box, begin, end));

	const auto middle = split(centroid_box, union_box, dimension, begin, end);

	// when the middle is equal to begin or end, it means the other node is empty
	// we can reduce the situation, make the node as leaf 
	if (middle == begin || middle == end)
		return &((*node) = bounding_volume_hierarchy_node(union_box, begin, end));

	(*node) = bounding_volume_hierarchy_node(
		recursive_build(begin, middle),
		recursive_build(middle, end),
		begin, end, dimension
	);

	return node;
}

size_t rainbow::accelerators::bounding_volume_hierarchy::split(
	const bounding_box& centroid_box, const bounding_box& union_box, 
	size_t dimension, size_t begin, size_t end)
{
	return split_equal_count(centroid_box, union_box, dimension, begin, end);
}

size_t rainbow::accelerators::bounding_volume_hierarchy::split_equal_count(
	const bounding_box& centroid_box, const bounding_box& union_box, 
	size_t dimension, size_t begin, size_t end)
{
	// split_equal_count will split boxes into two equal part
	// the first part has the (middle - begin) element that belongs the first (middle - begin) element in the order array
	// the second part has others elements.
	// notice : the order in first and second part is unknown.
	const auto middle = (begin + end) >> 1;

	std::nth_element(mBoundingBoxes.data() + begin, mBoundingBoxes.data() + middle,
		mBoundingBoxes.data() + end,
		[dimension](const bounding_box& left, const bounding_box& right)
		{
			return left.centroid()[static_cast<int>(dimension)] < right.centroid()[static_cast<int>(dimension)];
		});

	return middle;
}
