#pragma once

#include "../textures/texture.hpp"
#include "material.hpp"

namespace rainbow::cpus::materials {

	class mixture_material final : public material {
	public:
		explicit mixture_material(
			const std::shared_ptr<textures::texture2d<spectrum>> alpha,
			const std::shared_ptr<material>& material0,
			const std::shared_ptr<material>& material1);

		surface_properties build_surface_properties(
			const surface_interaction& interaction) const noexcept override;

		surface_properties build_surface_properties(
			const surface_interaction& interaction, const spectrum& scale) const noexcept override;
	private:
		std::shared_ptr<textures::texture2d<spectrum>> mAlpha;

		std::array<std::shared_ptr<material>, 2> mMaterials;
	};

}