#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/interactions/surface_interaction.hpp"
#include "../shared/transform.hpp"
#include "../shared/ray.hpp"

#include <optional>

namespace rainbow {

	using namespace interactions;
	
	namespace shapes {

		class shape : public interfaces::noncopyable {
		public:
			explicit shape(const transform& transform);

			~shape() = default;
			
			transform transform() const;

			virtual std::optional<surface_interaction> intersect(const ray& ray) = 0;
		protected:
			rainbow::transform mLocalToWorld, mWorldToLocal;
		}; 
		
	}
}