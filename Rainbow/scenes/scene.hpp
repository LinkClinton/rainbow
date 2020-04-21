#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../emitters/emitter.hpp"
#include "../shapes/shape.hpp"

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

			void add_shape(const std::shared_ptr<shape>& shape);

			void add_emitter(const std::shared_ptr<emitter>& emitter);
			
			std::optional<surface_interaction> intersect(const ray& ray) const;

			std::optional<surface_interaction> intersect_with_shadow_ray(const ray& ray) const;

			const std::vector<std::shared_ptr<emitter>>& emitters() const noexcept;
			
			const std::vector<std::shared_ptr<shape>>& shapes() const noexcept;
		private:
			std::vector<std::shared_ptr<emitter>> mEmitters;
			std::vector<std::shared_ptr<shape>> mShapes;
		};
		
	}
}
