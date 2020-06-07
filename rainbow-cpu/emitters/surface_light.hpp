#pragma once

#include "emitter.hpp"

namespace rainbow::cpus::emitters {

	class surface_light final : public emitter {
	public:
		explicit surface_light(const spectrum& radiance);

		~surface_light() = default;

		spectrum evaluate(const interaction& interaction, const vector3& wi) const override;

		emitter_sample sample(
			const std::shared_ptr<shape>& shape, const interaction& reference, const vector2& sample) const override;

		real pdf(
			const std::shared_ptr<shape>& shape, const interaction& reference, const vector3& wi) const override;

		spectrum power(
			const std::shared_ptr<shape>& shape) const override;
	private:
		spectrum mRadiance;
	};

}
