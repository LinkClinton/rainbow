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
				const std::shared_ptr<textures::texture2d<vector2>>& roughness,
				bool map_roughness_to_alpha = true);

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept override;
		private:
			std::shared_ptr<textures::texture2d<spectrum>> mEta;
			std::shared_ptr<textures::texture2d<spectrum>> mK;
			std::shared_ptr<textures::texture2d<vector2>> mRoughness;

			bool mMapRoughnessToAlpha = true;
		};
		
	}
}
