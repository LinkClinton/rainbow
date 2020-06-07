#pragma once

#include "utilities.hpp"

#include <string>
#include <vector>

namespace rainbow::core {
	
	class file_system final {
	public:
		static void write_image(const std::string& name, const std::vector<unsigned char>& data, int width, int height);
	};
	
}
