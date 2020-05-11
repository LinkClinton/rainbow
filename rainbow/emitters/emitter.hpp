#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/interactions/interaction.hpp"
#include "../shared/spectrums/spectrum.hpp"
#include "../shared/math/math.hpp"
#include "../shared/transform.hpp"

#include "../shapes/shape.hpp"

namespace rainbow {

	using namespace interactions;
	using namespace spectrums;
	using namespace shapes;
	using namespace math;
	
	namespace emitters {

		enum class emitter_type : uint32 {
			delta_position = 1 << 0,
			environment = 1 << 1,
			surface = 1 << 2,
		};

		emitter_type operator&(const emitter_type& lhs, const emitter_type& rhs);
		
		bool has(const emitter_type& type, const emitter_type& flag);

		struct emitter_sample {
			spectrum intensity = 0;
			vector3 position = vector3(0);
			vector3 wi = vector3(0);
			real pdf = 0;

			emitter_sample() = default;

			emitter_sample(const spectrum& intensity, const vector3& position, const vector3& wi, real pdf);

			static emitter_sample transform(const transform& transform, const emitter_sample& sample);
		};

		bool is_delta_emitter(const emitter_type& type);

		bool is_environment_emitter(const emitter_type& type);
		
		class emitter : public interfaces::noncopyable {
		public:
			using sample_type = emitter_sample;
		public:
			explicit emitter(const emitter_type& type);

			virtual spectrum evaluate(const interaction& interaction, const vector3& wi) const = 0;
			
			virtual emitter_sample sample(
				const std::shared_ptr<shape>& shape, const interaction& reference, const vector2& sample) const = 0;

			virtual real pdf(
				const std::shared_ptr<shape>& shape, const interaction& reference, const vector3& wi) const = 0;

			virtual spectrum power(
				const std::shared_ptr<shape>& shape) const = 0;
			
			emitter_type type() const noexcept;

			bool is_environment() const noexcept;
			
			bool is_delta() const noexcept;
		protected:
			
			emitter_type mType;
		};
		
	}
}