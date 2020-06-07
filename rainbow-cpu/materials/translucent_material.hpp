#pragma once

#include "../textures/texture.hpp"

#include "material.hpp"

namespace rainbow::cpus::materials {

	class translucent_material final : public material {
	public:
		explicit translucent_material(
			const std::shared_ptr<textures::texture2d<spectrum>>& transmission,
			const std::shared_ptr<textures::texture2d<spectrum>>& reflectance,
			const std::shared_ptr<textures::texture2d<spectrum>>& specular,
			const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
			const std::shared_ptr<textures::texture2d<real>>& roughness,
			bool map_roughness_to_alpha = true);

		~translucent_material() = default;

		surface_properties build_surface_properties(
			const surface_interaction& interaction) const noexcept override;

		surface_properties build_surface_properties(
			const surface_interaction& interaction, const spectrum& scale) const noexcept override;
	private:
		std::shared_ptr<textures::texture2d<spectrum>> mTransmission;
		std::shared_ptr<textures::texture2d<spectrum>> mReflectance;
		std::shared_ptr<textures::texture2d<spectrum>> mSpecular;
		std::shared_ptr<textures::texture2d<spectrum>> mDiffuse;
		std::shared_ptr<textures::texture2d<real>> mRoughness;

		bool mMapRoughnessToAlpha;
	};

}