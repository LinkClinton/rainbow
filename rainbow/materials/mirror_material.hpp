#pragma once

#include "../textures/texture.hpp"

#include "material.hpp"

namespace rainbow {

	namespace materials {

		class mirror_material final : public material {
		public:
			explicit mirror_material(
				const std::shared_ptr<textures::texture2d<spectrum>>& reflectance);

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept override;
		private:
			std::shared_ptr<textures::texture2d<spectrum>> mReflectance;
		};
		
	}
}
