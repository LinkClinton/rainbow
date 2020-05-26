#pragma once

#include "../accelerator.hpp"

#undef near
#undef far

namespace rainbow {

	namespace accelerators {

		template <typename T>
		bounding_box<T>::bounding_box(const std::shared_ptr<const T>& entity) :
			entity(entity), box(entity->bounding_box())
		{
		}

		template <typename T>
		bounding_box<T>::bounding_box(const vector3& v0, const vector3& v1) :
			box(v0, v1)
		{
		}

		template <typename T>
		bounding_box<T>::bounding_box(const bounding_box& box0, const bounding_box& box1)
		{
			box.min = min(box0.box.min, box1.box.min);
			box.max = max(box0.box.max, box1.box.max);
		}

		template <typename T>
		bool bounding_box<T>::intersect(const ray& ray) const
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

		template <typename T>
		void bounding_box<T>::union_it(const bounding_box& box) noexcept
		{
			this->box.min = min(this->box.min, box.box.min);
			this->box.max = max(this->box.max, box.box.max);
		}

		template <typename T>
		void bounding_box<T>::union_it(const vector3& point) noexcept
		{
			box.min = min(box.min, point);
			box.max = max(box.max, point);
		}

		template <typename T>
		size_t bounding_box<T>::max_dimension() const noexcept
		{
			const auto length = box.max - box.min;

			if (length.x > length.y && length.x > length.z) return 0;
			if (length.y > length.z) return 1;
			return 2;
		}

		template <typename T>
		vector3 bounding_box<T>::centroid() const noexcept
		{
			return (box.min + box.max) * static_cast<real>(0.5);
		}

		template <typename T>
		accelerator<T>::accelerator(const std::vector<bounding_box<T>>& boxes) :
			mBoundingBoxes(boxes)
		{
		}
		
	}
}
