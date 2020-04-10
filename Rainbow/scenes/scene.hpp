#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../shapes/shapes.hpp"
#include "../lights/light.hpp"

#include <memory>
#include <vector>

namespace rainbow {

	using namespace shapes;
	using namespace lights;
	
	namespace scenes {

		class scene : public interfaces::noncopyable {
		public:
			scene() = default;

			~scene() = default;

			void add_shape(const std::shared_ptr<shape>& shape);

			void add_light(const std::shared_ptr<light>& light);
			
			surface_interaction intersect(const ray& ray) const;
		private:
			std::vector<std::shared_ptr<shape>> mShapes;
			std::vector<std::shared_ptr<light>> mLights;
		};
		
	}
}
