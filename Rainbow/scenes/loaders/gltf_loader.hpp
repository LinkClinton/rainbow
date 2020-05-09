#pragma once

#include "../../shapes/mesh.hpp"

#include <string>
#include <memory>
#include <vector>

#define __GLTF_LOADER__

namespace rainbow {

	using namespace shapes;
	
	namespace scenes {

		namespace loaders {

#ifdef __GLTF_LOADER__
			std::vector<std::shared_ptr<mesh>> load_glb_mesh(const std::string& file);
#endif
			
		}		
	}
}
