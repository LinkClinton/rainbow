#pragma once

#include "light.hpp"

namespace rainbow {

	namespace lights {

		class point_light : public light {
		public:
			explicit point_light(const transform& transform, const spectrum& irradiance);

			light_sample sample(const vector3& point, const vector2& sample) override;

			real pdf(const vector3& point, const vector3& wi) override;
			
			spectrum power() override;
		private:
			spectrum mIrradiance;
			vector3 mPosition;
		};
		
	}
}
