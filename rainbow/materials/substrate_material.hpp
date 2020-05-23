#pragma once

#include "../textures/texture.hpp"
#include "material.hpp"

namespace rainbow {

	namespace materials {

		class substrate_material final : public material {
		public:
			explicit substrate_material(
				const std::shared_ptr<textures::texture2d<spectrum>>& specular,
				const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
				const std::shared_ptr<textures::texture2d<real>>& roughness_u,
				const std::shared_ptr<textures::texture2d<real>>& roughness_v,
				bool map_roughness_to_alpha = true);

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept override;
		private:
			std::shared_ptr<textures::texture2d<spectrum>> mSpecular;
			std::shared_ptr<textures::texture2d<spectrum>> mDiffuse;
			std::shared_ptr<textures::texture2d<real>> mRoughnessU;
			std::shared_ptr<textures::texture2d<real>> mRoughnessV;
			
			bool mMapRoughnessToAlpha = true;
		};
		
	}
}
