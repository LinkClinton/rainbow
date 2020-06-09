#pragma once

#include "medium.hpp"

namespace rainbow::cpus::media {

	class homogeneous_medium final : public medium {
	public:
		explicit homogeneous_medium(
			const spectrum& sigma_a, const spectrum& sigma_s, real g);

		spectrum evaluate(const std::shared_ptr<sampler1d>& sampler, const ray& ray) const override;

		medium_sample sample(const std::shared_ptr<sampler1d>& sampler, const ray& ray) const override;
	private:
		spectrum mSigmaA;
		spectrum mSigmaS;
		spectrum mSigmaT;

		real mG;
	};
	
}
