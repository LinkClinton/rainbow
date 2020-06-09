#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../samplers/samplers.hpp"

#include "../shared/interactions/medium_interaction.hpp"
#include "../shared/spectrums/spectrum.hpp"
#include "../shared/transform.hpp"
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

		static medium_sample transform(const transform& transform, const medium_sample& sample);
	};
	
	class medium : public interfaces::noncopyable {
	public:
		medium() = default;

		~medium() = default;

		virtual spectrum evaluate(const std::shared_ptr<sampler1d>& sampler, const ray& ray) const = 0;

		virtual medium_sample sample(const std::shared_ptr<sampler1d>& sampler, const ray& ray) const = 0;
	};

	class media final : public interfaces::noncopyable {
	public:
		using sample_type = medium_sample;
	public:
		explicit media(const std::shared_ptr<medium>& outside, const std::shared_ptr<medium>& inside);

		explicit media(const std::shared_ptr<medium>& two_side);

		~media() = default;
		
		std::shared_ptr<medium> outside() const noexcept;

		std::shared_ptr<medium> inside() const noexcept;

		bool different_sides() const noexcept;
	private:
		std::shared_ptr<medium> mOutside;
		std::shared_ptr<medium> mInSide;
	};
}
