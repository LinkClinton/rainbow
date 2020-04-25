#pragma once

#include "shape.hpp"

namespace rainbow {

	using namespace math;

	namespace shapes {

		class sphere : public shape {
		public:
			explicit sphere(real radius, bool reverse_orientation = false);

			~sphere() = default;

			std::optional<surface_interaction> intersect(const ray& ray) const override;

			shape_sample sample(const interaction& reference, const vector2& sample) const override;

			shape_sample sample(const vector2& sample) const override;

			real pdf(const interaction& reference, const vector3& wi) const override;

			real pdf() const override;

			real area() const noexcept override;
		private:
			real mRadius;
		};
		
	}
}
