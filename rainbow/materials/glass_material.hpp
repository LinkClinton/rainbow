#pragma once

#include "../textures/texture.hpp"

#include "material.hpp"

namespace rainbow {

	namespace materials {

		class glass_material final : public material {
		public:
			explicit glass_material(
				const std::shared_ptr<textures::texture2d<spectrum>>& reflectance,
				const std::shared_ptr<textures::texture2d<spectrum>>& transmission,
				const std::shared_ptr<textures::texture2d<real>>& roughness_u,
				const std::shared_ptr<textures::texture2d<real>>& roughness_v,
				const std::shared_ptr<textures::texture2d<real>>& eta,
				bool map_roughness_to_alpha = true);

			~glass_material() = default;

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept override;
		private:
			std::shared_ptr<textures::texture2d<spectrum>> mReflectance;
			std::shared_ptr<textures::texture2d<spectrum>> mTransmission;
			std::shared_ptr<textures::texture2d<real>> mRoughnessU;
			std::shared_ptr<textures::texture2d<real>> mRoughnessV;
			std::shared_ptr<textures::texture2d<real>> mEta;

			bool mMapRoughnessToAlpha;
		};
		
	}
}
