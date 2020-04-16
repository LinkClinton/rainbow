#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/spectrums/spectrum.hpp"
#include "../shared/math/math.hpp"
#include "../shared/transform.hpp"

namespace rainbow {

	using namespace spectrums;
	using namespace math;
	
	namespace lights {

		enum class light_type : uint32 {
			delta_position = 1 << 0
		};

		struct light_sample {
			spectrum irradiance = 0;
			vector3 position = vector3(0);
			vector3 wi = vector3(0);
			real pdf = 0;

			light_sample() = default;

			light_sample(const spectrum& irradiance, const vector3& position, const vector3& wi, real pdf);
		};

		class light : public interfaces::noncopyable {
		public:
			explicit light(const transform& transform, const light_type& type);

			virtual light_sample sample(const vector3& point, const vector2& sample) = 0;

			virtual real pdf(const vector3& point, const vector3& wi) = 0;

			virtual spectrum power() = 0;
			
			light_type type() const noexcept;
		protected:
			transform mLightToWorld, mWorldToLight;

			light_type mType;
		};
		
	}
}
