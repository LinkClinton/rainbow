#pragma once

#include "../textures/texture.hpp"
#include "material.hpp"

namespace rainbow::cpus::materials {

	class plastic_material final : public material {
	public:
		explicit plastic_material(
			const std::shared_ptr<textures::texture2d<spectrum>>& specular,
			const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
			const std::shared_ptr<textures::texture2d<real>>& roughness,
			bool map_roughness_to_alpha = true);

		explicit plastic_material(
			const std::shared_ptr<textures::texture2d<spectrum>>& specular,
			const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
			const std::shared_ptr<textures::texture2d<real>>& roughness,
			const std::shared_ptr<textures::texture2d<real>>& eta,
			bool map_roughness_to_alpha = true);

		~plastic_material() = default;

		surface_properties build_surface_properties(
			const surface_interaction& interaction) const noexcept override;

		surface_properties build_surface_properties(
			const surface_interaction& interaction, const spectrum& scale) const noexcept override;
	private:
		std::shared_ptr<textures::texture2d<spectrum>> mSpecular;
		std::shared_ptr<textures::texture2d<spectrum>> mDiffuse;
		std::shared_ptr<textures::texture2d<real>> mRoughness;
		std::shared_ptr<textures::texture2d<real>> mEta;

		bool mMapRoughnessToAlpha;
	};

}