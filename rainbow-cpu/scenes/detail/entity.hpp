#pragma once

#include "../entity.hpp"

namespace rainbow::cpus::scenes {

	/*
	 * entity:sample functions that use T to choose which object we will sample
	 * when we sample a object, we first transform the input from world to local
	 * and object will sample in local space
	 * at last, we will transform the output from local to world.
	 */

	template <>
	inline media::sample_type entity::sample<media>(const std::shared_ptr<sampler1d>& sampler, const interaction& interaction, const ray& ray) const
	{
		// the normal of surface indicate the outside medium, the ray's origin should be interaction.point
		// so if the interaction.normal dot interaction.wo > 0, means the ray is on the outside space
		// otherwise, the ray is on the inside space
		const auto medium = (dot(interaction.normal, interaction.wo) > 0) ? mMedia->outside() : mMedia->inside();

		// if the medium is nullptr, means it is vacuum.
		// we can not sample the medium interaction with vacuum medium
		// so we just return std::nullopt and 1 for beam
		if (medium == nullptr) return media::sample_type(std::nullopt, spectrum(1));
		
		return cpus::media::medium_sample::transform(mLocalToWorld,
			medium->sample(sampler, mWorldToLocal(ray))
		);
	}
	
	template <>
	inline shape::sample_type entity::sample<shape>(const interaction& reference, const vector2& sample) const
	{
		return shape_sample::transform(mLocalToWorld,
			mShape->sample(
				transform_interaction(mWorldToLocal, reference),
				sample
			));
	}

	template <>
	inline emitter::sample_type entity::sample<emitter>(const interaction& reference, const vector2& sample) const
	{
		return emitter_sample::transform(mLocalToWorld,
			mEmitter->sample(
				mShape,
				transform_interaction(mWorldToLocal, reference),
				sample
			));
	}

	template <>
	inline shape::sample_type entity::sample<shape>(const vector2& sample) const
	{
		return shape_sample::transform(mLocalToWorld, mShape->sample(sample));
	}

	template <>
	inline real entity::pdf<shape>(const interaction& reference, const vector3& wi) const
	{
		return mShape->pdf(
			transform_interaction(mWorldToLocal, reference),
			transform_vector(mWorldToLocal, wi));
	}

	template <>
	inline real entity::pdf<emitter>(const interaction& reference, const vector3& wi) const
	{
		return mEmitter->pdf(
			mShape,
			transform_interaction(mWorldToLocal, reference),
			transform_vector(mWorldToLocal, wi));
	}

	template <>
	inline spectrum entity::evaluate<media>(const std::shared_ptr<sampler1d>& sampler, const interaction& interaction, const ray& ray) const
	{
		// the normal of surface indicate the outside medium, the ray's origin should be interaction.point
		// so if the interaction.normal dot interaction.wo > 0, means the ray is on the outside space
		// otherwise, the ray is on the inside space
		const auto medium = (dot(interaction.normal, interaction.wo) > 0) ? mMedia->outside() : mMedia->inside();

		// if the medium is nullptr, means it is vacuum.
		// we just return 1 for beam
		if (medium == nullptr) return spectrum(1);
		
		return medium->evaluate(sampler, mWorldToLocal(ray));
	} 

	template <>
	inline spectrum entity::evaluate<emitter>(const interaction& interaction, const vector3& wi) const
	{
		return mEmitter->evaluate(
			transform_interaction(mWorldToLocal, interaction),
			transform_vector(mWorldToLocal, wi));
	}
	
	template <>
	inline real entity::pdf<shape>() const
	{
		return mShape->pdf();
	}

	template <>
	inline std::shared_ptr<material> entity::component() const noexcept
	{
		return mMaterial;
	}

	template <>
	inline std::shared_ptr<emitter> entity::component() const noexcept
	{
		return mEmitter;
	}

	template <>
	inline std::shared_ptr<shape> entity::component() const noexcept
	{
		return mShape;
	}

	template <>
	inline std::shared_ptr<media> entity::component() const noexcept
	{
		return mMedia;
	}

	template <typename T>
	bool entity::has_component() const noexcept
	{
		return component<T>() != nullptr;
	}
	
}