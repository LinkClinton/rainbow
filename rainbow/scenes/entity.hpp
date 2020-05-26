#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../materials/material.hpp"
#include "../emitters/emitter.hpp"
#include "../shapes/shape.hpp"

namespace rainbow {

	using namespace materials;
	using namespace emitters;
	using namespace shapes;
	
	namespace scenes {
	
		class entity final : public interfaces::noncopyable, public std::enable_shared_from_this<entity> {
		public:
			explicit entity(
				const std::shared_ptr<material>& material,
				const std::shared_ptr<emitter>& emitter,
				const std::shared_ptr<shape>& shape,
				const transform& transform);

			~entity() = default;

			std::optional<surface_interaction> intersect(const ray& ray) const;

			bound3 bounding_box() const;
			
			bool visible() const noexcept;

			spectrum power() const noexcept;
			
			template <typename T>
			spectrum evaluate(const interaction& interaction, const vector3& wi) const;
			
			template <typename T>
			typename T::sample_type sample(const interaction& reference, const vector2& sample) const;

			template <typename T>
			typename T::sample_type sample(const vector2& sample) const;

			template <typename T>
			real pdf(const interaction& reference, const vector3& wi) const;
			
			template <typename T>
			real pdf() const;
			
			template <typename T>
			std::shared_ptr<T> component() const noexcept;
			
			template <typename T>
			bool has_component() const noexcept;
		private:
			std::shared_ptr<material> mMaterial;
			std::shared_ptr<emitter> mEmitter;
			std::shared_ptr<shape> mShape;

			transform mLocalToWorld, mWorldToLocal;
		};
	}
}

#include "detail/entity.hpp"