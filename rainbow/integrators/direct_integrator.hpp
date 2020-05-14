#pragma once

#include "sampler_integrator.hpp"

namespace rainbow {

	namespace integrators {

		class direct_integrator final : public sampler_integrator {
		public:
			explicit direct_integrator(
				const std::shared_ptr<sampler2d>& sampler2d,
				const std::shared_ptr<sampler1d>& sampler1d,
				size_t emitter_samples = 4, size_t bsdf_samples = 4);

			~direct_integrator() = default;

			spectrum trace(
				const std::shared_ptr<scene>& scene, 
				const integrator_debug_info& debug, 
				const sampler_group& samplers, 
				const ray& ray, size_t depth) override;
		protected:
			sampler_group prepare_samplers(uint64 seed) override;
		private:
			std::shared_ptr<sampler1d> mSampler1D;

			size_t mEmitterSamples;
			size_t mBSDFSamples;

			real mFractionalEmitterSamples;
			real mFractionalBSDFSamples;
			
			real mWeightEmitterSamples;
			real mWeightBSDFSamples;
		};
		
	}
}
