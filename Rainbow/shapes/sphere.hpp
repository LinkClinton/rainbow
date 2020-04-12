#pragma once

#include "shape.hpp"

namespace rainbow {

	using namespace math;

	namespace shapes {

		class sphere : public shape {
		public:
			explicit sphere(
				const std::shared_ptr<materials::material>& material,
				const rainbow::transform& transform, real radius);

			~sphere() = default;

			std::optional<surface_interaction> intersect(const ray& ray) override;
		private:
			real mRadius;
		};
		
	}
}
