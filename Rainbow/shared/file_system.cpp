#include "file_system.hpp"

#include "logs/log.hpp"

#define __STB_IMAGE__

#ifdef __STB_IMAGE__

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image_write.h>
#include <stb_image.h>

#endif

void rainbow::file_system::write_image(const std::string& name, const std::vector<unsigned char>& data, int width, int height)
{
#ifdef __STB_IMAGE__
	stbi_write_png(name.c_str(), width, height, 4, data.data(), 0);
#else
	logs::error("no method to write image.");
#endif
}

rainbow::image_info rainbow::file_system::read_image(const std::string& name, bool gamma)
{
#ifdef __STB_IMAGE__
	auto channel = 0;
	auto width = 0;
	auto height = 0;
	
	image_info info;
	
	const auto data = stbi_load(name.c_str(), &width, &height, &channel, STBI_rgb_alpha);

	info.width = width;
	info.height = height;
	info.data = std::vector<real>(info.width * info.height * 4);

	for (size_t index = 0; index < info.data.size(); index++) {
		const auto value = static_cast<real>(data[index]) / 255;

		info.data[index] = gamma ? inverse_gamma_correct(value) : value;
	}
	
	stbi_image_free(data);

	return info;
#else
	logs::error("no method to read image.");
	return {};
#endif
}

rainbow::image_info rainbow::file_system::read_image_hdr(const std::string& name, bool gamma)
{
#ifdef __STB_IMAGE__
	auto channel = 0;
	auto width = 0;
	auto height = 0;

	image_info info;

	const auto data = stbi_loadf(name.c_str(), &width, &height, &channel, STBI_rgb_alpha);

	info.width = width;
	info.height = height;
	info.data = std::vector<real>(info.width * info.height * 4);

	for (size_t index = 0; index < info.data.size(); index++) 
		info.data[index] = gamma ? inverse_gamma_correct(data[index]) : data[index];

	stbi_image_free(data);

	return info;
#else
	logs::error("no method to read image.");
	return {};
#endif
}

