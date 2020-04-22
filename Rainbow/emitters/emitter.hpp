#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/interactions/interaction.hpp"
#include "../shared/spectrums/spectrum.hpp"
#include "../shared/math/math.hpp"
#include "../shared/transform.hpp"

namespace rainbow {

	using namespace interactions;
	using namespace spectrums;
	using namespace math;
	
	namespace emitters {

		enum class emitter_type : uint32 {
			delta_position = 1 << 0,
			surface = 1 << 2,
		};

		struct emitter_sample {
			spectrum irradiance = 0;
			vector3 position = vector3(0);
			vector3 wi = vector3(0);
			real pdf = 0;

			emitter_sample() = default;

			emitter_sample(const spectrum& irradiance, const vector3& position, const vector3& wi, real pdf);

			static emitter_sample transform(const transform& transform, const emitter_sample& sample);
		};

		class emitter : public interfaces::noncopyable {
		public:
			explicit emitter(const transform& transform, const emitter_type& type);

			virtual emitter_sample sample(const interaction& reference, const vector2& sample) = 0;

			virtual real pdf(const interaction& reference, const vector3& wi) = 0;

			virtual spectrum power() = 0;
			
			emitter_type type() const noexcept;
		protected:
			transform mEmitterToWorld, mWorldToEmitter;

			emitter_type mType;
		};
		
	}
}
