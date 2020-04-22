#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../emitters/emitter.hpp"
#include "../shapes/shape.hpp"
#include "entity.hpp"

#include <memory>
#include <vector>

namespace rainbow {

	using namespace emitters;
	using namespace shapes;
	
	namespace scenes {

		class scene : public interfaces::noncopyable {
		public:
			scene() = default;

			~scene() = default;

			void add_entity(const std::shared_ptr<entity>& entity);
			
			std::optional<surface_interaction> intersect(const ray& ray) const;

			std::optional<surface_interaction> intersect_with_shadow_ray(const ray& ray) const;

			const std::vector<std::shared_ptr<entity>>& emitters() const noexcept;
		private:
			std::vector<std::shared_ptr<entity>> mEntities;
			std::vector<std::shared_ptr<entity>> mEmitters;
		};
		
	}
}
