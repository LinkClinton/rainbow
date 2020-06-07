#pragma once

#include "../textures/texture.hpp"

#include "material.hpp"

namespace rainbow::cpus::materials {

	class mirror_material final : public material {
	public:
		explicit mirror_material(
			const std::shared_ptr<textures::texture2d<spectrum>>& reflectance);

		surface_properties build_surface_properties(
			const surface_interaction& interaction) const noexcept override;

		surface_properties build_surface_properties(
			const surface_interaction& interaction, const spectrum& scale) const noexcept override;
	private:
		std::shared_ptr<textures::texture2d<spectrum>> mReflectance;
	};

}