#pragma once

#include "emitter.hpp"

namespace rainbow {

	namespace emitters {

		class point_light : public emitter {
		public:
			explicit point_light(const transform& transform, const spectrum& irradiance);

			emitter_sample sample(const vector3& point, const vector2& sample) override;

			real pdf(const vector3& point, const vector3& wi) override;
			
			spectrum power() override;
		private:
			spectrum mIrradiance;
			vector3 mPosition;
		};
		
	}
}
