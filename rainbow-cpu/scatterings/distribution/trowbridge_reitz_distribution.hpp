#pragma once

#include "microfacet_distribution.hpp"

namespace rainbow::cpus::scatterings {

	class trowbridge_reitz_distribution final : public microfacet_distribution {
	public:
		explicit trowbridge_reitz_distribution(
			real alpha_x, real alpha_y, bool sample_visible_area = true);

		~trowbridge_reitz_distribution() = default;

		real distribution(const vector3& wh) const override;

		vector3 sample(const vector3& wo, const vector2& sample) override;

		static real roughness_to_alpha(real roughness);
	private:
		real lambda(const vector3& w) const override;

		real mAlphaX;
		real mAlphaY;
	};

}