#pragma once

#include "../scattering_function.hpp"

namespace rainbow {

	namespace scatterings {

		class reflection_function : public scattering_function {
		public:
			explicit reflection_function(const scattering_type& type, const spectrum& reflectance);

			~reflection_function() = default;

			scattering_sample sample(const vector3& wo, const vector2& sample) const override;

			real pdf(const vector3& wo, const vector3& wi) const override;
		protected:
			spectrum mReflectance;
		};
		
	}
}
