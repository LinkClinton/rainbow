#pragma once

#include "../../interfaces/noncopyable.hpp"
#include "../../shared/math/math.hpp"

namespace rainbow {

	using namespace math;
	
	namespace scatterings {

		/*
		 * we can divide the surface into some microfacet with different surface normal
		 * most surfaces in real world are not perfect smooth
		 * so we can use microfacet distribution to describe the distribution of these microfacet
		 *
		 * microfacet_distribution::distribution(wh) will return the probability of microfacet has normal wh.
		 * microfacet_distribution::masking_shadowing(wo, wi) will return the probability is visible of wo and wi
		 * microfacet_distribution::sample(wo, sample) will sample the wh.
		 */
		
		class microfacet_distribution : public interfaces::noncopyable {
		public:
			explicit microfacet_distribution(bool sample_visible_area = true);

			~microfacet_distribution() = default;

			real masking_shadowing(const vector3& w) const;

			real masking_shadowing(const vector3& wo, const vector3& wi) const;

			real pdf(const vector3& wo, const vector3& wh) const;
			
			virtual real distribution(const vector3& wh) const = 0;

			virtual vector3 sample(const vector3& wo, const vector2& sample) = 0;
		protected:
			virtual real lambda(const vector3& w) const = 0;
		
			bool mSampleVisibleArea;
		};
		
	}
}
