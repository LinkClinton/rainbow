#pragma once

#include "../phases/phase_function.hpp"

#include "interaction.hpp"

#include <memory>

namespace rainbow::cpus::shared::interactions {

	using namespace phases;
	
	struct medium_interaction final : interaction {
		std::shared_ptr<phase_function> function;

		medium_interaction() = default;

		medium_interaction(
			const std::shared_ptr<phase_function>& function,
			const vector3& point, const vector3& wo);
	};
	
}
