#include "medium.hpp"

rainbow::cpus::media::medium_sample::medium_sample(const std::optional<medium_interaction>& interaction,
	const spectrum& value) : interaction(interaction), value(value)
{	
}

rainbow::cpus::media::medium_sample rainbow::cpus::media::medium_sample::transform(const shared::transform& transform,
	const medium_sample& sample)
{
	if (!sample.interaction.has_value()) return medium_sample(std::nullopt, sample.value);

	return medium_sample(
		transform(sample.interaction.value()),
		sample.value
	);
}

rainbow::cpus::media::media::media(const std::shared_ptr<medium>& outside, const std::shared_ptr<medium>& inside) :
	mOutside(outside), mInSide(inside)
{
}

rainbow::cpus::media::media::media(const std::shared_ptr<medium>& two_side) : media(two_side, two_side)
{
}

std::shared_ptr<rainbow::cpus::media::medium> rainbow::cpus::media::media::outside() const noexcept
{
	return mOutside;
}

std::shared_ptr<rainbow::cpus::media::medium> rainbow::cpus::media::media::inside() const noexcept
{
	return mInSide;
}

bool rainbow::cpus::media::media::different_sides() const noexcept
{
	return mOutside != mInSide;
}
