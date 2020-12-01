#pragma once

#include "../bounding_volume_hierarchy.hpp"

#include <algorithm>
#include <stack>

#define BOUNDING_VOLUME_HIERARCHY_POOL_SIZE 16

namespace rainbow::cpus::shared::accelerators {

	template <typename T>
	bounding_volume_hierarchy_node<T>::bounding_volume_hierarchy_node(const bounding_box<T>& box, size_t begin, size_t end) :
		box(box), begin(begin), end(end)
	{
	}

	template <typename T>
	bounding_volume_hierarchy_node<T>::bounding_volume_hierarchy_node(
		bounding_volume_hierarchy_node* left,
		bounding_volume_hierarchy_node* right,
		size_t begin, size_t end, size_t axis) :
		right(right), left(left), box(left->box, right->box), begin(begin), end(end), axis(axis)
	{
	}

	template <typename T>
	bool bounding_volume_hierarchy_node<T>::is_leaf() const noexcept
	{
		return left == nullptr && right == nullptr;
	}

	template <typename T>
	bounding_volume_hierarchy_allocator<T>::bounding_volume_hierarchy_allocator()
	{
		mMemoryPools.push_back(std::vector<bounding_volume_hierarchy_node<T>>(BOUNDING_VOLUME_HIERARCHY_POOL_SIZE));
	}

	template <typename T>
	bounding_volume_hierarchy<T>::bounding_volume_hierarchy(const std::vector<bounding_box<T>>& boxes) :
		accelerator<T>(boxes), mRoot(recursive_build(0, boxes.size()))
	{
	}

	template <typename T>
	bounding_volume_hierarchy_node<T>* bounding_volume_hierarchy_allocator<T>::allocate()
	{
		// when the nodes greater than the poll size, we will create a new pool to allocate the memory
		// we do not need a full consecutive pool of nodes
		if (mCurrentNodes == mMemoryPools.back().size()) {
			mMemoryPools.push_back(std::vector<bounding_volume_hierarchy_node<T>>(BOUNDING_VOLUME_HIERARCHY_POOL_SIZE));

			mCurrentNodes = 0;
		}

		return &mMemoryPools.back()[mCurrentNodes++];
	}


	template <typename T>
	std::optional<surface_interaction> bounding_volume_hierarchy<T>::intersect(const ray& ray) const
	{
		std::stack<bounding_volume_hierarchy_node<T>*> stack;
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
						this->mBoundingBoxes[index].entity->intersect(ray);

					if (interaction.has_value()) nearest_interaction = interaction;
				}

				continue;
			}

			// if the direction of ray is negative, the near child should be the right child
			// if the direction of ray is not negative, the near child should be the left child
			// we will travel the near child firstly
			if (is_negative_direction[node->axis]) {
				stack.push(node->left); stack.push(node->right);
			}
			else {
				stack.push(node->right); stack.push(node->left);
			}
		}

		return nearest_interaction;
	}

	template <typename T>
	std::optional<surface_interaction> bounding_volume_hierarchy<T>::intersect_with_shadow_ray(const ray& ray) const
	{
		std::stack<bounding_volume_hierarchy_node<T>*> stack;
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
					if (!this->mBoundingBoxes[index].entity->visible()) continue;

					const auto interaction =
						this->mBoundingBoxes[index].entity->intersect(ray);

					if (interaction.has_value()) nearest_interaction = interaction;
				}

				continue;
			}

			// if the direction of ray is negative, the near child should be the right child
			// if the direction of ray is not negative, the near child should be the left child
			// we will travel the near child firstly
			if (is_negative_direction[node->axis]) {
				stack.push(node->left); stack.push(node->right);
			}
			else {
				stack.push(node->right); stack.push(node->left);
			}
		}

		return nearest_interaction;
	}

	template <typename T>
	bounding_volume_hierarchy_node<T>* bounding_volume_hierarchy<T>::recursive_build(size_t begin, size_t end)
	{
		const auto node = mAllocator.allocate();

		auto union_box = this->mBoundingBoxes[begin];

		// union all boxes in these node, this box shows the space of this node
		for (auto index = begin + 1; index < end; index++)
			union_box.union_it(this->mBoundingBoxes[index]);

		// if there is only one entity in this node, the node should be leaf
		if (end - begin == 1) return &((*node) = bounding_volume_hierarchy_node(union_box, begin, end));

		auto centroid_box = bounding_box<T>(
			this->mBoundingBoxes[begin].centroid(),
			this->mBoundingBoxes[begin].centroid()
			);

		// union centroid of all boxes to create ta centroid_box
		for (auto index = begin + 1; index < end; index++)
			centroid_box.union_it(this->mBoundingBoxes[index].centroid());

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

	template <typename T>
	size_t bounding_volume_hierarchy<T>::split(const bounding_box<T>& centroid_box, const bounding_box<T>& union_box,
		size_t dimension, size_t begin, size_t end)
	{
		return split_equal_count(centroid_box, union_box, dimension, begin, end);
	}

	template <typename T>
	size_t bounding_volume_hierarchy<T>::split_equal_count(
		const bounding_box<T>& centroid_box, const bounding_box<T>& union_box, size_t dimension, size_t begin, size_t end)
	{
		// split_equal_count will split boxes into two equal part
		// the first part has the (middle - begin) element that belongs the first (middle - begin) element in the order array
		// the second part has others elements.
		// notice : the order in first and second part is unknown.
		const auto middle = (begin + end) >> 1;

		std::nth_element(this->mBoundingBoxes.data() + begin, this->mBoundingBoxes.data() + middle,
			this->mBoundingBoxes.data() + end,
			[dimension](const bounding_box<T>& left, const bounding_box<T>& right)
			{
				return left.centroid()[static_cast<int>(dimension)] < right.centroid()[static_cast<int>(dimension)];
			});

		return middle;
	}

	template <typename T>
	size_t bounding_volume_hierarchy<T>::split_surface_area_heuristic(const bounding_box<T>& centroid_box,
		const bounding_box<T>& union_box, size_t dimension, size_t begin, size_t end)
	{
		if (end - begin <= 4) return split_equal_count(centroid_box, union_box, dimension, begin, end);

		constexpr size_t buckets_count = 12;

		std::vector<bucket_info> buckets(buckets_count);

		for (auto index = begin; index < end; index++) {
			auto location = static_cast<size_t>(
				(this->mBoundingBoxes[index].centroid()[static_cast<int>(dimension)] - centroid_box.min()[static_cast<int>(dimension)]) /
				(centroid_box.max()[static_cast<int>(dimension)] - centroid_box.min()[static_cast<int>(dimension)]) * buckets_count);

			if (location == buckets_count) location = location - 1;

			buckets[location].box.union_it(this->mBoundingBoxes[index]);
			buckets[location].count = buckets[location].count + 1;
		}

		auto min_cost_location = static_cast<size_t>(0);
		auto min_cost = cost_surface_area_heuristic(buckets, union_box, 0);

		for (size_t index = 1; index < buckets_count - 1; index++) {
			const auto cost = cost_surface_area_heuristic(buckets, union_box, index);

			if (min_cost > cost) min_cost = cost, min_cost_location = index;
		}

		const auto leaf_cost = end - begin;

		if (min_cost < leaf_cost || max_elements_one_node < leaf_cost) {
			auto middle = std::partition(this->mBoundingBoxes.data() + begin, this->mBoundingBoxes.data() + end,
				[&](const bounding_box<T>& box)
				{
					auto location = static_cast<size_t>(
						(box.centroid()[static_cast<int>(dimension)] - centroid_box.min()[static_cast<int>(dimension)]) /
						(centroid_box.max()[static_cast<int>(dimension)] - centroid_box.min()[static_cast<int>(dimension)]) * buckets_count);

					if (location == buckets_count) location = location - 1;

					return location <= min_cost_location;
				});

			return middle - this->mBoundingBoxes.data();
		}

		// leaf node
		return begin;
	}

	template <typename T>
	real bounding_volume_hierarchy<T>::cost_surface_area_heuristic(const std::vector<bucket_info>& infos,
		const bounding_box<T>& union_box, size_t location)
	{
		assert(location >= 0 && location < infos.size());

		constexpr auto travel_cost = static_cast<real>(0.125);
		constexpr auto test_cost = static_cast<real>(1);

		bucket_info info0;
		bucket_info info1;

		for (size_t index = 0; index <= location; index++) {
			info0.box.union_it(infos[index].box);

			info0.count = info0.count + infos[index].count;
		}

		for (size_t index = location + 1; index < infos.size(); index++) {
			info1.box.union_it(infos[index].box);

			info1.count = info1.count + infos[index].count;
		}

		return travel_cost + test_cost * (info0.count * info0.box.area() + info1.count * info1.box.area()) / union_box.area();
	}

}