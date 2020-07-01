#pragma once

#include "integrator.hpp"

namespace rainbow::cpus::integrators {

	class bidirectional_path_integrator final : public integrator {
	public:
		explicit bidirectional_path_integrator(
			const std::shared_ptr<sampler2d>& sampler2d,
			const std::shared_ptr<sampler1d>& sampler1d,
			size_t max_depth = 5);

		~bidirectional_path_integrator() = default;

		void render(
			const std::shared_ptr<camera>& camera, 
			const std::shared_ptr<scene>& scene) override;
	private:
		std::shared_ptr<sampler2d> mSampler2D;
		std::shared_ptr<sampler1d> mSampler1D;

		size_t mMaxDepth;
	};
	
}
