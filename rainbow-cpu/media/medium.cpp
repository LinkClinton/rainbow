#include "medium.hpp"

#include "../scenes/entity.hpp"

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

rainbow::cpus::media::medium_info::medium_info(
	const std::shared_ptr<const scenes::entity>& entity,
	const std::shared_ptr<const cpus::media::medium>& medium) :
	entity(entity), medium(medium)
{
}

rainbow::cpus::media::medium_info::medium_info(
	const std::shared_ptr<const scenes::entity>& entity,
	const vector3& normal, const vector3& wi) :
	entity(entity)
{
	if (dot(normal, wi) > 0)
		medium = entity->component<media>()->outside();
	else
		medium = entity->component<media>()->inside();
}

rainbow::cpus::shared::spectrums::spectrum rainbow::cpus::media::medium_info::evaluate(
	const std::shared_ptr<sampler1d>& sampler, const ray& ray) const
{
	// if the medium is nullptr, means it is vacuum.
	// so we just return 1 for the beam
	if (medium == nullptr) return spectrum(1);

	return medium->evaluate(sampler, ray);
}

rainbow::cpus::media::medium_sample rainbow::cpus::media::medium_info::sample(
	const std::shared_ptr<sampler1d>& sampler, const ray& ray) const
{
	// if the medium is nullptr, means it is vacuum.
	// we can not sample the medium interaction with vacuum medium
	// so we just return std::nullopt and 1 for the beam
	if (medium == nullptr) return medium_sample(std::nullopt, spectrum(1));

	return medium->sample(sampler, ray);
}

rainbow::cpus::media::medium_info rainbow::cpus::media::medium_info::clone(const vector3& normal,
	const vector3& wi) const
{
	if (entity == nullptr || !entity->has_component<media>()) return medium_info(entity, nullptr);

	return medium_info(entity, normal, wi);
}

bool rainbow::cpus::media::medium_info::has() const noexcept
{
	return medium != nullptr && entity != nullptr;
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
