#pragma once

#include "emitter.hpp"

namespace rainbow::cpus::emitters {

	class directional_light final : public emitter {
	public:
		explicit directional_light(const spectrum& irradiance, const vector3& direction, real radius);

		~directional_light() = default;

		spectrum evaluate(const interaction& interaction, const vector3& wi) const override;

		emitter_ray_sample sample(const shape_instance_properties& properties, const vector2& sample0, const vector2& sample1) const override;

		emitter_sample sample(const shape_instance_properties& properties, const interaction& reference, const vector2& sample) const override;

		std::tuple<real, real> pdf(const shape_instance_properties& properties, const ray& ray, const vector3& normal) const override;

		real pdf(const shape_instance_properties& properties, const interaction& reference, const vector3& wi) const override;

		spectrum power(const shape_instance_properties& properties) const override;
	private:
		spectrum mIrradiance;
		vector3 mDirection;

		real mRadius;
	};

}