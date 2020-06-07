#pragma once

#include "../scatterings/scattering_function_collection.hpp"

#include "../shared/interactions/surface_interaction.hpp"

#include "../interfaces/noncopyable.hpp"

namespace rainbow::cpus::scatterings {
	class bidirectional_scattering_surface_distribution_function;

	using scattering_surface_function = bidirectional_scattering_surface_distribution_function;
}

namespace rainbow::cpus::materials {

	using namespace scatterings;

	struct surface_properties {
		std::shared_ptr<scattering_surface_function> bssrdf;

		scattering_function_collection functions;

		surface_properties() = default;
	};

	class material : public interfaces::noncopyable {
	public:
		material() = default;

		virtual surface_properties build_surface_properties(
			const surface_interaction& interaction) const noexcept = 0;

		virtual surface_properties build_surface_properties(
			const surface_interaction& interaction, const spectrum& scale) const noexcept = 0;
	};

}