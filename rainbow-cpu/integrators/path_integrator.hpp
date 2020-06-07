#pragma once

#include "sampler_integrator.hpp"

namespace rainbow::cpus::integrators {

	class path_integrator final : public sampler_integrator {
	public:
		explicit path_integrator(
			const std::shared_ptr<sampler2d>& sampler2d,
			const std::shared_ptr<sampler1d>& sampler1d,
			size_t max_depth = 5,
			real threshold = 1);

		~path_integrator() = default;

		spectrum trace(
			const std::shared_ptr<scene>& scene,
			const integrator_debug_info& debug,
			const sampler_group& samplers,
			const ray& first_ray,
			size_t depth) override;
	protected:
		sampler_group prepare_samplers(uint64 seed) override;
	private:
		std::shared_ptr<sampler1d> mSampler1D;

		real mThreshold = static_cast<real>(1.0);
	};

}