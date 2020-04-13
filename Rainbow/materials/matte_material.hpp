#pragma once

#include "../textures/texture.hpp"

#include "material.hpp"

namespace rainbow {

	using namespace textures;
	
	namespace materials {

		class matte_material final : public material {
		public:
			explicit matte_material(
				const std::shared_ptr<texture2d<spectrum>>& diffuse,
				const std::shared_ptr<texture2d<real>>& sigma);

			~matte_material() = default;

			scattering_function_collection build_scattering_functions(
				const surface_interaction& interaction) const noexcept override;
		private:
			std::shared_ptr<texture2d<spectrum>> mDiffuse;
			std::shared_ptr<texture2d<real>> mSigma;
		};
		
	}
}
