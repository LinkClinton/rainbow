#pragma once

#include "shape.hpp"

namespace rainbow {

	namespace shapes {

		class disk : public shape {
		public:
			explicit disk(
				const std::shared_ptr<materials::material>& material,
				const rainbow::transform& transform, real radius, real height = 0);

			~disk() = default;

			std::optional<surface_interaction> intersect(const ray& ray) override;

			shape_sample sample(const interaction& reference, const vector2& sample) const override;

			shape_sample sample(const vector2& sample) const override;

			real pdf(const interaction& reference, const vector3& wi) const override;

			real pdf() const override;

			real area() const noexcept override;
		private:
			real mHeight;
			real mRadius;
		};
		
	}
}
