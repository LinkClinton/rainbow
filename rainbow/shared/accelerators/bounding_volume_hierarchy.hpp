#pragma once

#include "accelerator.hpp"

namespace rainbow {

	namespace accelerators {

		struct bounding_volume_hierarchy_node {
			bounding_volume_hierarchy_node* right = nullptr;
			bounding_volume_hierarchy_node* left = nullptr;

			bounding_box box;

			size_t begin = 0;
			size_t end = 0;
			size_t axis = 0;
			
			bounding_volume_hierarchy_node() = default;

			bounding_volume_hierarchy_node(const bounding_box& box, size_t begin, size_t end);

			bounding_volume_hierarchy_node(
				bounding_volume_hierarchy_node* left,
				bounding_volume_hierarchy_node* right,
				size_t begin, size_t end, size_t axis);
			
			bool is_leaf() const noexcept;
		};

		class bounding_volume_hierarchy_allocator final : public interfaces::noncopyable {
		public:
			bounding_volume_hierarchy_allocator();
			
			~bounding_volume_hierarchy_allocator() = default;

			bounding_volume_hierarchy_node* allocate();
		private:
			std::vector<std::vector<bounding_volume_hierarchy_node>> mMemoryPools;

			size_t mCurrentNodes = 0;
		};
		
		class bounding_volume_hierarchy final : public accelerator {
		public:
			explicit bounding_volume_hierarchy(const std::vector<bounding_box>& boxes);

			std::optional<surface_interaction> intersect(const ray& ray) const override;

			std::optional<surface_interaction> intersect_with_shadow_ray(const ray& ray) const override;
		private:
			bounding_volume_hierarchy_node* recursive_build(size_t begin, size_t end);

			size_t split(const bounding_box& centroid_box, const bounding_box& union_box, 
				size_t dimension, size_t begin, size_t end);

			size_t split_equal_count(const bounding_box& centroid_box, const bounding_box& union_box,
				size_t dimension, size_t begin, size_t end);
			
			bounding_volume_hierarchy_allocator mAllocator;
			bounding_volume_hierarchy_node* mRoot;
		};
		
	}
}
