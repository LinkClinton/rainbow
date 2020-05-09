#pragma once

#include "../textures/texture.hpp"

#include "material.hpp"

namespace rainbow {

	namespace materials {

		class plastic_material final : public material {
		public:
			explicit plastic_material(
				const std::shared_ptr<textures::texture2d<spectrum>>& specular,
				const std::shared_ptr<textures::texture2d<spectrum>>& diffuse,
				const std::shared_ptr<textures::texture2d<real>>& roughness,
				bool map_roughness_to_alpha = true);

			~plastic_material() = default;

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept override;

		private:
			std::shared_ptr<textures::texture2d<spectrum>> mSpecular;
			std::shared_ptr<textures::texture2d<spectrum>> mDiffuse;
			std::shared_ptr<textures::texture2d<real>> mRoughness;

			bool mMapRoughnessToAlpha;
		};
		
	}
}
