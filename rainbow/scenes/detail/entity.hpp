#pragma once

#include "../entity.hpp"

namespace rainbow {

	namespace scenes {

		/*
		 * entity:sample functions that use T to choose which object we will sample
		 * when we sample a object, we first transform the input from world to local
		 * and object will sample in local space
		 * at last, we will transform the output from local to world.
		 */
		
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

		template <typename T>
		bool entity::has_component() const noexcept
		{
			return component<T>() != nullptr;
		}
		
	}
}
