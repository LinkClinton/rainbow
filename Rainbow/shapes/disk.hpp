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
		private:
			real mHeight;
			real mRadius;
		};
		
	}
}
