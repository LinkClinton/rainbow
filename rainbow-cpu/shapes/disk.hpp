#pragma once

#include "shape.hpp"

namespace rainbow::cpus::shapes {

	class disk final : public shape {
	public:
		explicit disk(real radius, real height = 0, bool reverse_orientation = false);

		~disk() = default;

		std::optional<surface_interaction> intersect(const ray& ray, size_t index) const override;

		std::optional<surface_interaction> intersect(const ray& ray) const override;

		bound3 bounding_box(const transform& transform, size_t index) const override;

		bound3 bounding_box(const transform& transform) const override;

		shape_sample sample(const shape_instance_properties& properties, const vector2& sample) const override;

		real pdf(const shape_instance_properties& properties) const override;

		real area(const transform& transform, size_t index) const noexcept override;
		
		real area(const transform& transform) const noexcept override;
		
		real area(size_t index) const noexcept override;

		real area() const noexcept override;

		void build_accelerator() override;
	private:
		real mHeight;
		real mRadius;
	};

}
