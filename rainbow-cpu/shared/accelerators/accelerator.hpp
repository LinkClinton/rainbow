#pragma once

#include "../../../rainbow-core/math/math.hpp"

#include "../interactions/surface_interaction.hpp"
#include "../../interfaces/noncopyable.hpp"
#include "../ray.hpp"

#include <optional>
#include <memory>
#include <vector>

namespace rainbow::cpus::shared::accelerators {

	using namespace interactions;
	using namespace core::math;
	
	template <typename T>
	struct bounding_box {
		std::shared_ptr<const T> entity = nullptr;

		bound3 box = bound3();

		bounding_box() = default;

		bounding_box(const std::shared_ptr<const T>& entity);

		bounding_box(const vector3& v0, const vector3& v1);

		bounding_box(const bounding_box& box0, const bounding_box& box1);

		bool intersect(const ray& ray) const;

		void union_it(const bounding_box& box) noexcept;

		void union_it(const vector3& point) noexcept;

		size_t max_dimension() const noexcept;

		vector3 centroid() const noexcept;

		vector3 max() const noexcept;

		vector3 min() const noexcept;

		real area() const noexcept;
	};

	template <typename T>
	class accelerator : public interfaces::noncopyable {
	public:
		explicit accelerator(const std::vector<bounding_box<T>>& boxes);

		virtual std::optional<surface_interaction> intersect(const ray& ray) const = 0;

		virtual std::optional<surface_interaction> intersect_with_shadow_ray(const ray& ray) const = 0;
	protected:
		std::vector<bounding_box<T>> mBoundingBoxes;
	};

}

#include "detail/accelerator.hpp"