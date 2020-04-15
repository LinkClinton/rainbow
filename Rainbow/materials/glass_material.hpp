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
				const std::shared_ptr<textures::texture2d<vector2>>& roughness,
				const std::shared_ptr<textures::texture2d<real>>& eta);

			~glass_material() = default;

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept override;
		private:
			std::shared_ptr<textures::texture2d<spectrum>> mReflectance;
			std::shared_ptr<textures::texture2d<spectrum>> mTransmission;
			std::shared_ptr<textures::texture2d<vector2>> mRoughness;
			std::shared_ptr<textures::texture2d<real>> mEta;
		};
		
	}
}
