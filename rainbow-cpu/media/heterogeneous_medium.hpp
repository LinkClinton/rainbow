#pragma once

#include "../textures/texture.hpp"

#include "medium.hpp"

namespace rainbow::cpus::media {

	class heterogeneous_medium final : public medium {
	public:
		explicit heterogeneous_medium(
			const std::shared_ptr<textures::texture3d<spectrum>>& albedo,
			const std::shared_ptr<textures::texture3d<real>>& sigma_t,
			const std::shared_ptr<textures::texture3d<real>>& g,
			const transform& transform);

		~heterogeneous_medium() = default;

		spectrum evaluate(const std::shared_ptr<sampler1d>& sampler, const ray& ray) const override;

		medium_sample sample(const std::shared_ptr<sampler1d>& sampler, const ray& ray) const override;
	private:
		std::tuple<real, real, bool> intersect(const bound3& box, const ray& ray) const;
	private:
		std::shared_ptr<textures::texture3d<spectrum>> mAlbedo;
		std::shared_ptr<textures::texture3d<real>> mSigmaT;
		std::shared_ptr<textures::texture3d<real>> mG;

		transform mLocalToWorld;
		transform mWorldToLocal;
		
		real mMaxDensity, mInvMaxDensity;
	};
	
}
