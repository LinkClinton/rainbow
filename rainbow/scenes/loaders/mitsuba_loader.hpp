#pragma once

#include "../scene.hpp"

#include "../../integrators/integrator.hpp"
#include "../../cameras/camera.hpp"

#include <unordered_map>

#define __MITSUBA_LOADER__

namespace rainbow {

	using namespace integrators;
	using namespace cameras;
	
	namespace scenes {

		namespace loaders {

			struct scene_info {
				std::unordered_map<std::string, std::shared_ptr<material>> materials;

				std::shared_ptr<integrator> integrator;
				std::shared_ptr<camera> camera;
				std::shared_ptr<scene> scene;
				std::shared_ptr<film> film;

				size_t sample_count = 16;

				std::string directory;
				
				scene_info() = default;

				void build();

				void render() const;

				void write(const std::string& file_name) const;
			};
			
#ifdef __MITSUBA_LOADER__

			scene_info load_mitsuba_scene(const std::string& file_name);
			
#endif
			
		}	
	}
}
