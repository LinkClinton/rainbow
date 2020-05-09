#pragma once

#include "reflection_function.hpp"

namespace rainbow {

	namespace scatterings {

		class oren_nayar_reflection final : public reflection_function {
		public:
			explicit oren_nayar_reflection(const spectrum& reflectance, real sigma);

			~oren_nayar_reflection() = default;
			
			spectrum evaluate(const vector3& wo, const vector3& wi) const override;
		private:
			real mA, mB;
		};
		
	}
}
