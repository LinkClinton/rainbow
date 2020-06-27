#pragma once

#include "integrator.hpp"

namespace rainbow::cpus::integrators {

	class photon_mapping_integrator final : public integrator {
	public:
		explicit photon_mapping_integrator(
			const std::shared_ptr<sampler2d>& sampler2d,
			const std::shared_ptr<sampler1d>& sampler1d,
			size_t iterations = 64, size_t max_depth = 5,
			size_t photons = 0, real radius = 1);

		~photon_mapping_integrator() = default;

		void render(
			const std::shared_ptr<camera>& camera, 
			const std::shared_ptr<scene>& scene) override;
	private:
		std::shared_ptr<sampler2d> mSampler2D;
		std::shared_ptr<sampler1d> mSampler1D;
		
		size_t mIterations = 64;
		size_t mMaxDepth = 5;
		size_t mPhotons = 0;

		real mRadius = 1;
	};
	
}
