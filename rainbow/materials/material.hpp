#pragma once

#include "../scatterings/scattering_function_collection.hpp"

#include "../shared/interactions/surface_interaction.hpp"

#include "../interfaces/noncopyable.hpp"


namespace rainbow {

	using namespace scatterings;
	
	namespace materials {

		class material : public interfaces::noncopyable {
		public:
			material() = default;

			virtual scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept = 0;

			virtual scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction, const spectrum& scale) const noexcept = 0;
		};
		
	}
}
