#pragma once

#include "emitter.hpp"

namespace rainbow {

	namespace emitters {

		class directional_light final : public emitter {
		public:
			explicit directional_light(const spectrum& irradiance, const vector3& direction, real radius);

			~directional_light() = default;

			spectrum evaluate(const interaction& interaction, const vector3& wi) const override;

			emitter_sample sample(
				const std::shared_ptr<shape>& shape, const interaction& reference, const vector2& sample) const override;

			real pdf(
				const std::shared_ptr<shape>& shape, const interaction& reference, const vector3& wi) const override;

			spectrum power(const std::shared_ptr<shape>& shape) const override;
		private:
			spectrum mIrradiance;
			vector3 mDirection;

			real mRadius;
		};
		
	}
}
