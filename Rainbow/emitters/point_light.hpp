#pragma once

#include "emitter.hpp"

namespace rainbow {

	namespace emitters {

		class point_light : public emitter {
		public:
			explicit point_light(const transform& transform, const spectrum& intensity);

			emitter_sample sample(const interaction& reference, const vector2& sample) override;

			real pdf(const interaction& reference, const vector3& wi) override;
			
			spectrum power() override;
		private:
			spectrum mIntensity;
			vector3 mPosition;
		};
		
	}
}
