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
		inline shape_sample entity::sample(const interaction& reference, const vector2& sample) const
		{
			return shape_sample::transform(mLocalToWorld,
				mShape->sample(
					transform_interaction(mWorldToLocal, reference),
					sample
				));
		}

		template <>
		inline emitter_sample entity::sample(const interaction& reference, const vector2& sample) const
		{
			return emitter_sample::transform(mLocalToWorld,
				mEmitter->sample(
					transform_interaction(mWorldToLocal, reference),
					sample
				));
		}

		template <>
		inline shape_sample entity::sample(const vector2& sample) const
		{
			return shape_sample::transform(mLocalToWorld, mShape->sample(sample));
		}

		template <>
		inline real entity::pdf<shape_sample>(const interaction& reference, const vector3& wi) const
		{
			return mShape->pdf(
				transform_interaction(mWorldToLocal, reference),
				transform_vector(mWorldToLocal, wi));
		}

		template <>
		inline real entity::pdf<emitter_sample>(const interaction& reference, const vector3& wi) const
		{
			return mEmitter->pdf(
				transform_interaction(mWorldToLocal, reference),
				transform_point(mWorldToLocal, wi));
		}

		template <>
		inline real entity::pdf<shape_sample>() const
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
