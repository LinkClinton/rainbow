#include "file_system.hpp"

#include "logs/log.hpp"

#define __STB_IMAGE__

#ifdef __STB_IMAGE__

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>

#endif

void rainbow::file_system::write_image(const std::string& name, const std::vector<unsigned char>& data, int width, int height)
{
#ifdef __STB_IMAGE__
	stbi_write_png(name.c_str(), width, height, 4, data.data(), 0);
#else
	logs::error("no method to write image.");
#endif
}