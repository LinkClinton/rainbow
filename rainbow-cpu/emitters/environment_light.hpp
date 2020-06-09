#pragma once

#include "../shared/distributions/distribution.hpp"
#include "../textures/texture.hpp"

#include "emitter.hpp"

namespace rainbow::cpus::emitters {

	using namespace distributions;
	using namespace textures;

	class environment_light final : public emitter {
	public:
		explicit environment_light(const spectrum& intensity, real radius);

		explicit environment_light(
			const std::shared_ptr<texture2d<spectrum>>& environment_map,
			const spectrum& intensity,
			real radius);

		~environment_light() = default;

		spectrum evaluate(const interaction& interaction, const vector3& wi) const override;

		emitter_sample sample(const std::shared_ptr<shape>& shape, const interaction& reference, const vector2& sample) const override;

		real pdf(const std::shared_ptr<shape>& shape, const interaction& reference, const vector3& wi) const override;

		spectrum power(const std::shared_ptr<shape>& shape) const override;

		real radius() const noexcept;
	private:
		std::shared_ptr<texture2d<spectrum>> mEnvironmentMap;
		std::shared_ptr<distribution2d> mDistribution;

		spectrum mIntensity;

		real mRadius;
	};
	
}
