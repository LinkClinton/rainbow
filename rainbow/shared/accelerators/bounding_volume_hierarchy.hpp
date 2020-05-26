#pragma once

#include "accelerator.hpp"

namespace rainbow {

	namespace accelerators {

		template <typename T>
		struct bounding_volume_hierarchy_node {
			bounding_volume_hierarchy_node* right = nullptr;
			bounding_volume_hierarchy_node* left = nullptr;

			bounding_box<T> box;

			size_t begin = 0;
			size_t end = 0;
			size_t axis = 0;
			
			bounding_volume_hierarchy_node() = default;

			bounding_volume_hierarchy_node(const bounding_box<T>& box, size_t begin, size_t end);

			bounding_volume_hierarchy_node(
				bounding_volume_hierarchy_node* left,
				bounding_volume_hierarchy_node* right,
				size_t begin, size_t end, size_t axis);
			
			bool is_leaf() const noexcept;
		};

		template <typename T>
		class bounding_volume_hierarchy_allocator final : public interfaces::noncopyable {
		public:
			bounding_volume_hierarchy_allocator();
			
			~bounding_volume_hierarchy_allocator() = default;

			bounding_volume_hierarchy_node<T>* allocate();
		private:
			std::vector<std::vector<bounding_volume_hierarchy_node<T>>> mMemoryPools;
			std::vector<std::shared_ptr<bounding_volume_hierarchy_node<T>>> mNodes;

			size_t mCurrentNodes = 0;
		};

		template <typename T>
		class bounding_volume_hierarchy final : public accelerator<T> {
		public:
			explicit bounding_volume_hierarchy(const std::vector<bounding_box<T>>& boxes);

			std::optional<surface_interaction> intersect(const ray& ray) const override;

			std::optional<surface_interaction> intersect_with_shadow_ray(const ray& ray) const override;
		private:
			bounding_volume_hierarchy_node<T>* recursive_build(size_t begin, size_t end);

			size_t split(const bounding_box<T>& centroid_box, const bounding_box<T>& union_box, 
				size_t dimension, size_t begin, size_t end);

			size_t split_equal_count(const bounding_box<T>& centroid_box, const bounding_box<T>& union_box,
				size_t dimension, size_t begin, size_t end);
			
			bounding_volume_hierarchy_allocator<T> mAllocator;
			bounding_volume_hierarchy_node<T>* mRoot;
		};
		
	}
}

#include "detail/bounding_volume_hierarchy.hpp"