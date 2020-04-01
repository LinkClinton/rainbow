#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/interactions/surface_interaction.hpp"
#include "../shared/transform.hpp"
#include "../shared/ray.hpp"

namespace rainbow {

	using namespace interactions;
	
	namespace shapes {

		class shape : public interfaces::noncopyable {
		public:
			shape() = default;

			explicit shape(const transform& transform);

			transform transform() const;

			virtual surface_interaction intersect(const ray& ray) = 0;
		protected:
			rainbow::transform mTransform;
		}; 
		
	}
}