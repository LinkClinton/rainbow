#pragma once

#include "sampler_integrator.hpp"

namespace rainbow {

	namespace integrators {

		class whitted_integrator : public sampler_integrator {
		public:
			explicit whitted_integrator(
				const std::shared_ptr<sampler2d>& camera_sampler,
				const std::shared_ptr<sampler2d>& sampler,
				size_t max_depth = 5);

			~whitted_integrator() = default;
			
			spectrum trace(
				const std::shared_ptr<scene>& scene,
				const integrator_debug_info& debug,
				const sampler_group& samplers, 
				const ray& ray, 
				size_t depth) override;
		protected:
			sampler_group prepare_samplers(uint64 seed) override;
		private:
			spectrum specular_reflect(
				const std::shared_ptr<scene>& scene, 
				const integrator_debug_info& debug, 
				const sampler_group& samplers,
				const ray& ray, const surface_interaction& interaction, 
				const scattering_function_collection& functions, size_t depth);

			spectrum specular_refract(
				const std::shared_ptr<scene>& scene,
				const integrator_debug_info& debug,
				const sampler_group& samplers,
				const ray& ray, const surface_interaction& interaction, 
				const scattering_function_collection& functions, size_t depth);

			std::shared_ptr<sampler2d> mSampler;
		};
		
	}
}
