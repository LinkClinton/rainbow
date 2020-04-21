#pragma once

#include "emitter.hpp"

#include "../shapes/shape.hpp"

namespace rainbow {

	namespace emitters {

		class surface_light final : public emitter {
		public:
			explicit surface_light(
				const std::shared_ptr<shape>& surface,
				const spectrum& radiance);

			~surface_light() = default;

			spectrum evaluate(const interaction& interaction, const vector3& wi) const noexcept;

			std::shared_ptr<shape> surface() const noexcept;
			
			emitter_sample sample(const interaction& reference, const vector2& sample) override;

			real pdf(const interaction& reference, const vector3& wi) override;
			
			spectrum power() override;
		private:
			std::shared_ptr<shape> mSurface;
			
			spectrum mRadiance;

			real mSurfaceArea;
		};
		
	}
}
