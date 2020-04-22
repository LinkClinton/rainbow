#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../materials/material.hpp"

#include "../shared/interactions/surface_interaction.hpp"
#include "../shared/transform.hpp"
#include "../shared/ray.hpp"

#include <optional>

namespace rainbow {

	using namespace interactions;
	using namespace materials;
	
	namespace shapes {

		struct shape_sample {
			interaction interaction;
			real pdf = 0;

			shape_sample() = default;

			shape_sample(
				const interactions::interaction& interaction,
				real pdf);
		};
		
		class shape : public interfaces::noncopyable, public std::enable_shared_from_this<shape> {
		public:
			explicit shape(
				const std::shared_ptr<material>& material,
				const transform& transform);

			~shape() = default;
			
			transform transform() const;

			std::shared_ptr<material> material() const noexcept;

			virtual std::optional<surface_interaction> intersect(const ray& ray) const = 0;

			virtual shape_sample sample(const interaction& reference, const vector2& sample) const;

			virtual shape_sample sample(const vector2& sample) const = 0;
			
			virtual real pdf(const interaction& reference, const vector3& wi) const;

			virtual real pdf() const = 0;

			virtual real area() const noexcept = 0;
		protected:
			std::shared_ptr<materials::material> mMaterial;
			
			rainbow::transform mLocalToWorld, mWorldToLocal;
		}; 
		
	}
}