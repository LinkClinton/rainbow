#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/spectrums/spectrum.hpp"
#include "../samplers/samplers.hpp"
#include "../cameras/camera.hpp"
#include "../scenes/scene.hpp"

namespace rainbow {

	using namespace spectrums;
	using namespace samplers;
	using namespace cameras;
	using namespace scenes;
	
	namespace integrators {

		struct integrator_debug_info {
			vector2i pixel = vector2i(0);

			integrator_debug_info() = default;

			integrator_debug_info(const vector2i& pixel);
		};

		struct sampler_group {
			std::shared_ptr<sampler1d> sampler1d;
			std::shared_ptr<sampler2d> sampler2d;

			sampler_group() = default;

			sampler_group(
				const std::shared_ptr<samplers::sampler1d>& sampler1d,
				const std::shared_ptr<samplers::sampler2d>& sampler2d);

			void reset() const noexcept;
		};
		
		class integrator : public interfaces::noncopyable {
		public:
			integrator() = default;

			~integrator() = default;

			virtual void render(
				const std::shared_ptr<camera>& camera,
				const std::shared_ptr<scene>& scene) = 0;
		};

		spectrum uniform_sample_one_light(
			const std::shared_ptr<scene>& scene, const sampler_group& samplers,
			const surface_interaction& interaction,
			const scattering_function_collection& functions);

		spectrum estimate_lighting(
			const std::shared_ptr<scene>& scene,
			const std::shared_ptr<light>& light,
			const sampler_group& samplers,
			const surface_interaction& interaction,
			const scattering_function_collection& functions);
	}
}
