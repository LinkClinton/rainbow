#pragma once

#include "shape.hpp"

#include <array>

namespace rainbow::cpus::shapes {

	class curve final : public shape {
	public:
		explicit curve(
			const std::array<vector3, 4>& control_points,
			const std::array<real, 2>& width,
			real u_min, real u_max);

		~curve() = default;

		std::optional<surface_interaction> intersect(const ray& ray, size_t index) const override;

		std::optional<surface_interaction> intersect(const ray& ray) const override;

		bound3 bounding_box(const transform& transform, size_t index) const override;

		bound3 bounding_box(const transform& transform) const override;

		shape_sample sample(const vector2& sample) const override;

		real pdf() const override;

		real area(size_t index) const noexcept override;

		real area() const noexcept override;

		void build_accelerator() override;
	private:
		std::optional<surface_interaction> recursive_intersect(const std::array<vector3, 4>& control_points,
			const ray& ray, const transform& ray_to_local, real u_min, real u_max, size_t depth) const;
	private:
		std::array<vector3, 4> mControlPoints;
		std::array<real, 2> mWidth;

		real mUMin;
		real mUMax;
	};
	
}
