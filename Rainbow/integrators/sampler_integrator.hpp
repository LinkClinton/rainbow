#pragma once

#include "integrator.hpp"

namespace rainbow {

	namespace integrators {

		class sampler_integrator : public integrator {
		public:
			explicit sampler_integrator(
				const std::shared_ptr<sampler2d>& sampler2d,
				size_t max_depth = 5);

			~sampler_integrator() = default;

			void render(
				const std::shared_ptr<camera>& camera,
				const std::shared_ptr<scene>& scene) override;

			virtual spectrum trace(
				const std::shared_ptr<scene>& scene,
				const integrator_debug_info& debug,
				const sampler_group& samplers,
				const ray& ray, size_t depth) = 0;
		protected:
			virtual sampler_group prepare_samplers(uint64 seed);

			std::shared_ptr<sampler2d> mSampler2D;
			
			const size_t mMaxDepth = 5;
		};

		
	}
}
