#pragma once

#include "../textures/texture.hpp"
#include "material.hpp"

namespace rainbow {

	namespace materials {

		class metal_material final : public material {
		public:
			explicit metal_material(
				const std::shared_ptr<textures::texture2d<spectrum>>& eta,
				const std::shared_ptr<textures::texture2d<spectrum>>& k,
				const std::shared_ptr<textures::texture2d<real>>& roughness_u,
				const std::shared_ptr<textures::texture2d<real>>& roughness_v,
				bool map_roughness_to_alpha = true);

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept override;

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction, const spectrum& scale) const noexcept override;
		private:
			std::shared_ptr<textures::texture2d<spectrum>> mEta;
			std::shared_ptr<textures::texture2d<spectrum>> mK;
			std::shared_ptr<textures::texture2d<real>> mRoughnessU;
			std::shared_ptr<textures::texture2d<real>> mRoughnessV;

			bool mMapRoughnessToAlpha = true;
		};
		
	}
}
