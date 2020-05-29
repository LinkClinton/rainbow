#pragma once

#include "../textures/texture.hpp"
#include "material.hpp"

namespace rainbow {
	
	namespace materials {

		class mixture_material final : public material {
		public:
			explicit mixture_material(
				const std::shared_ptr<textures::texture2d<spectrum>> alpha,
				const std::shared_ptr<material>& material0,
				const std::shared_ptr<material>& material1);

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept override;

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction, const spectrum& scale) const noexcept override;
		private:
			std::shared_ptr<textures::texture2d<spectrum>> mAlpha;
			
			std::array<std::shared_ptr<material>, 2> mMaterials;
		};
		
	}
}
