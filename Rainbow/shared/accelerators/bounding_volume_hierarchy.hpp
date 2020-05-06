#pragma once

#include "accelerator.hpp"

namespace rainbow {

	namespace accelerators {

		struct bounding_volume_hierarchy_node {
			bounding_volume_hierarchy_node* right = nullptr;
			bounding_volume_hierarchy_node* left = nullptr;

			bounding_box bounding_box;
	
			size_t begin = 0;
			size_t end = 0;

			bounding_volume_hierarchy_node() = default;

			bool is_leaf() const noexcept;
		};
		
		class bounding_volume_hierarchy final : public accelerator {
		public:
			explicit bounding_volume_hierarchy(const std::vector<bounding_box>& boxes);

			std::optional<surface_interaction> intersect(const ray& ray) const override;

			std::optional<surface_interaction> intersect_with_shadow_ray(const ray& ray) const override;
		private:
		};
		
	}
}
