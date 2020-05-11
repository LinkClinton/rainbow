#pragma once

#include "../../interfaces/noncopyable.hpp"
#include "../../scenes/entity.hpp"
#include "../math/math.hpp"

namespace rainbow {

	using namespace math;
	using namespace scenes;
	
	namespace accelerators {

		struct bounding_box {
			std::shared_ptr<const entity> entity = nullptr;
			
			bound3 box = bound3();

			size_t index = 0;

			bounding_box() = default;

			bounding_box(const std::shared_ptr<const scenes::entity>& entity, size_t index = 0);

			bounding_box(const vector3& v0, const vector3& v1);

			bounding_box(const bounding_box& box0, const bounding_box& box1);

			bool intersect(const ray& ray) const;
			
			void union_it(const bounding_box& box) noexcept;

			void union_it(const vector3& point) noexcept;

			size_t max_dimension() const noexcept;
			
			vector3 centroid() const noexcept;
		};
		
		class accelerator : public interfaces::noncopyable {
		public:
			explicit accelerator(const std::vector<bounding_box>& boxes);

			virtual std::optional<surface_interaction> intersect(const ray& ray) const = 0;

			virtual std::optional<surface_interaction> intersect_with_shadow_ray(const ray& ray) const = 0;
		protected:
			std::vector<bounding_box> mBoundingBoxes;
		};
		
	}
}