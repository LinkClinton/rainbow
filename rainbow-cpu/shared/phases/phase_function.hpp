#pragma once

#include "../../../rainbow-core/math/math.hpp"

#include "../../interfaces/noncopyable.hpp"
#include "../interactions/interaction.hpp"

namespace rainbow::cpus::shared::phases {

	using namespace interactions;
	using namespace core::math;
	using namespace core;

	struct phase_sample {
		vector3 wi = vector3(0);

		real value = 0;

		phase_sample() = default;

		phase_sample(const vector3& wi, real value);
	};
	
	class phase_function : public interfaces::noncopyable {
	public:
		phase_function() = default;

		~phase_function() = default;

		virtual real evaluate(const vector3& wo, const vector3& wi) const = 0;

		virtual phase_sample sample(const interaction& interaction, const vector2& sample) const = 0;
	};
	
}
