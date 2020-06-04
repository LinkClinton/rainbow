#pragma once

#include "../textures/texture.hpp"
#include "material.hpp"

namespace rainbow {

	namespace materials {

		class subsurface_material final : public material {
		public:
			explicit subsurface_material(
				const std::shared_ptr<textures::texture2d<spectrum>>& transmission,
				const std::shared_ptr<textures::texture2d<spectrum>>& reflectance,
				const std::shared_ptr<textures::texture2d<spectrum>>& sigma_a,
				const std::shared_ptr<textures::texture2d<spectrum>>& sigma_s,
				const std::shared_ptr<textures::texture2d<real>>& roughness_u,
				const std::shared_ptr<textures::texture2d<real>>& roughness_v,
				const std::shared_ptr<textures::texture2d<real>>& eta,
				real scale = 1, bool map_roughness_to_alpha = true);

			~subsurface_material() = default;

			surface_properties build_surface_properties(
				const surface_interaction& interaction) const noexcept override;

			surface_properties build_surface_properties(
				const surface_interaction& interaction, const spectrum& scale) const noexcept override;
		private:
			std::shared_ptr<textures::texture2d<spectrum>> mTransmission;
			std::shared_ptr<textures::texture2d<spectrum>> mReflectance;
			std::shared_ptr<textures::texture2d<spectrum>> mSigmaA;
			std::shared_ptr<textures::texture2d<spectrum>> mSigmaS;
			std::shared_ptr<textures::texture2d<real>> mRoughnessU;
			std::shared_ptr<textures::texture2d<real>> mRoughnessV;
			std::shared_ptr<textures::texture2d<real>> mEta;

			real mScale;
			
			bool mMapRoughnessToAlpha;
		};
		
	}
}
