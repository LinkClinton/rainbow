#pragma once

#include "../textures/texture.hpp"
#include "material.hpp"

namespace rainbow {

	namespace materials {

		class uber_material final : public material {
		public:
			explicit uber_material(
				const std::shared_ptr<textures::texture2d<spectrum>>& transmission,
				const std::shared_ptr<textures::texture2d<spectrum>>& reflectance,
				const std::shared_ptr<textures::texture2d<spectrum>>& specular,
				const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
				const std::shared_ptr<textures::texture2d<spectrum>>& opacity,
				const std::shared_ptr<textures::texture2d<real>>& roughness_u,
				const std::shared_ptr<textures::texture2d<real>>& roughness_v,
				const std::shared_ptr<textures::texture2d<real>>& eta,
				bool map_roughness_to_alpha = true);

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept override;

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction, const spectrum& scale) const noexcept override;
		private:
			std::shared_ptr<textures::texture2d<spectrum>> mTransmission;
			std::shared_ptr<textures::texture2d<spectrum>> mReflectance;
			std::shared_ptr<textures::texture2d<spectrum>> mSpecular;
			std::shared_ptr<textures::texture2d<spectrum>> mDiffuse;
			std::shared_ptr<textures::texture2d<spectrum>> mOpacity;

			std::shared_ptr<textures::texture2d<real>> mRoughnessU;
			std::shared_ptr<textures::texture2d<real>> mRoughnessV;
			std::shared_ptr<textures::texture2d<real>> mEta;

			bool mMapRoughnessToAlpha;
		};
		
	}
}
