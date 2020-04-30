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

		static image_info read_image(const std::string& name, bool gamma = false);

		static image_info read_image_hdr(const std::string& name, bool gamma = false);
		
		template <typename T>
		static std::shared_ptr<image_texture2d<T>> read_texture2d(const std::string& name, bool gamma = false);

		template <typename T>
		static std::shared_ptr<image_texture2d<T>> read_texture2d_hdr(const std::string& name, bool gamma = false);
	};

	template <>
	inline std::shared_ptr<image_texture2d<spectrum>> file_system::read_texture2d_hdr(const std::string& name, bool gamma)
	{
		const auto image_info = read_image_hdr(name, false);

		auto values = std::vector<spectrum>(image_info.width * image_info.height);

		for (size_t index = 0; index < values.size(); index++) {
			values[index].coefficient[0] = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 0]) : image_info.data[index * 4 + 0];
			values[index].coefficient[1] = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 1]) : image_info.data[index * 4 + 1];
			values[index].coefficient[2] = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 2]) : image_info.data[index * 4 + 2];
		}

		return std::make_shared<image_texture2d<spectrum>>(texture_size<2>(image_info.width, image_info.height), values);
	}

	template <>
	inline std::shared_ptr<image_texture2d<real>> file_system::read_texture2d(const std::string& name, bool gamma)
	{
		const auto image_info = read_image(name, false);

		auto values = std::vector<real>(image_info.width * image_info.height);

		for (size_t index = 0; index < values.size(); index++) 
			values[index] = gamma ? inverse_gamma_correct(image_info.data[index * 4]) : image_info.data[index * 4];

		return std::make_shared<image_texture2d<real>>(texture_size<2>(image_info.width, image_info.height), values);
	}

	template <>
	inline std::shared_ptr<image_texture2d<vector2>> file_system::read_texture2d(const std::string& name, bool gamma)
	{
		const auto image_info = read_image(name, false);

		auto values = std::vector<vector2>(image_info.width * image_info.height);

		for (size_t index = 0; index < values.size(); index++) {
			values[index].x = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 0]) : image_info.data[index * 4 + 0];
			values[index].y = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 1]) : image_info.data[index * 4 + 1];
		}
			
		return std::make_shared<image_texture2d<vector2>>(texture_size<2>(image_info.width, image_info.height), values);
	}

	template <>
	inline std::shared_ptr<image_texture2d<spectrum>> file_system::read_texture2d(const std::string& name, bool gamma)
	{
		const auto image_info = read_image(name, false);

		auto values = std::vector<spectrum>(image_info.width * image_info.height);

		for (size_t index = 0; index < values.size(); index++) {
			values[index].coefficient[0] = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 0]) : image_info.data[index * 4 + 0];
			values[index].coefficient[1] = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 1]) : image_info.data[index * 4 + 1];
			values[index].coefficient[2] = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 2]) : image_info.data[index * 4 + 2];
		}

		return std::make_shared<image_texture2d<spectrum>>(texture_size<2>(image_info.width, image_info.height), values);
	}
}
