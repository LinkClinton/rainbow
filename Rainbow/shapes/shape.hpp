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

		class shape : public interfaces::noncopyable, public std::enable_shared_from_this<shape> {
		public:
			explicit shape(
				const std::shared_ptr<material>& material,
				const transform& transform);

			~shape() = default;
			
			transform transform() const;

			std::shared_ptr<material> material() const noexcept;
			
			virtual std::optional<surface_interaction> intersect(const ray& ray) = 0;
		protected:
			std::shared_ptr<materials::material> mMaterial;
			
			rainbow::transform mLocalToWorld, mWorldToLocal;
		}; 
		
	}
}