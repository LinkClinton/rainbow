#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../samplers/samplers.hpp"

#include "../shared/interactions/medium_interaction.hpp"
#include "../shared/spectrums/spectrum.hpp"
#include "../shared/ray.hpp"

#include <optional>

namespace rainbow::cpus::media {

	using namespace samplers;
	using namespace interactions;
	using namespace spectrums;
	using namespace shared;

	struct medium_sample {
		std::optional<medium_interaction> interaction = std::nullopt;
		
		spectrum value = spectrum(0);

		medium_sample() = default;

		medium_sample(
			const std::optional<medium_interaction>& interaction,
			const spectrum& value);
	};
	
	class medium : public interfaces::noncopyable {
	public:
		medium() = default;

		~medium() = default;

		virtual spectrum evaluate(const std::shared_ptr<sampler1d>& sampler, const ray& ray) = 0;

		virtual medium_sample sample(const std::shared_ptr<sampler1d>& sampler, const ray& ray) = 0;
	};
	
}
