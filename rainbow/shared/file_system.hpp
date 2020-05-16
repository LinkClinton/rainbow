#pragma once

#include "../textures/image_texture.hpp"

#include "spectrums/spectrum.hpp"
#include "utilities.hpp"

#include <string>
#include <vector>

namespace rainbow {

	using namespace spectrums;
	using namespace textures;
	
	struct image_info {
		std::vector<real> data;

		size_t height;
		size_t width;
	};
	
	class file_system final {
	public:
		static void write_image(const std::string& name, const std::vector<unsigned char>& data, int width, int height);
	};
}
