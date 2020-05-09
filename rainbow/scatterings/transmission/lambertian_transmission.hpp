#pragma once

#include "transmission_function.hpp"

namespace rainbow {

	namespace scatterings {

		class lambertian_transmission final : public transmission_function {
		public:
			explicit lambertian_transmission(const spectrum& transmission);

			~lambertian_transmission() = default;

			spectrum evaluate(const vector3& wo, const vector3& wi) const override;
		};
		
	}
}
