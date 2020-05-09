#include "mitsuba_loader.hpp"

#include "../../integrators/path_integrator.hpp"
#include "../../cameras/perspective_camera.hpp"
#include "../../filters/gaussian_filter.hpp"
#include "../../shared/logs/log.hpp"

#include <sstream>

#ifdef __MITSUBA_LOADER__

#define __OBJ_LOADER__

#include "obj_loader.hpp"
#include <tinyxml2.h>

#endif

#ifdef __MITSUBA_LOADER__

const std::string MITSUBA_INTEGRATOR_ELEMENT = "integrator";
const std::string MITSUBA_TRANSFORM_ELEMENT = "transform";
const std::string MITSUBA_SAMPLER_ELEMENT = "sampler";
const std::string MITSUBA_INTEGER_ELEMENT = "integer";
const std::string MITSUBA_FILTER_ELEMENT = "rfilter";
const std::string MITSUBA_SENSOR_ELEMENT = "sensor";
const std::string MITSUBA_FLOAT_ELEMENT = "float";
const std::string MITSUBA_FILM_ELEMENT = "film";

namespace rainbow::scenes::loaders {

	real read_real(const std::string& value)
	{
		if constexpr (sizeof(real) == 4)
			return static_cast<real>(std::stof(value));

		if constexpr (sizeof(real) == 8)
			return static_cast<real>(std::stod(value));

		return 0;
	}

	template <typename T>
	constexpr bool is_integral() {
		return std::is_integral<T>::value && !std::is_unsigned<T>::value;
	}

	template <typename T>
	constexpr bool is_unsigned_integral() {
		return std::is_integral<T>::value && std::is_unsigned<T>::value;
	}
	
	template <typename T>
	T read_integer(const std::string& value)
	{
		if constexpr (is_integral<T>() && sizeof(T) == 4)
			return std::stoi(value);
		
		if constexpr (is_unsigned_integral<T>() && sizeof(T) == 8)
			return std::stoull(value);

		logs::error("the format of integer we are not support!");
		
		return 0;
	}
	
	vector3 read_vector3(const std::string& value)
	{
		std::stringstream stream(value);

		vector3 ret;

		for (auto index = 0; index < 3; index++) {
			std::string number; std::getline(stream, number, ',');

			ret[index] = read_real(number);
		}

		return ret;
	}

	void loop_all_children(const tinyxml2::XMLNode* node, const std::function<void(const tinyxml2::XMLNode*)>& function)
	{
		auto current = node->FirstChild();

		while (current != nullptr) {
			function(current);

			current = current->NextSibling();
		}
	}
	
	void process_transform(const tinyxml2::XMLNode* node, transform& transform)
	{
		const auto element = node->FirstChildElement();

		// mitsuba 0.6 use "lookAt" but mitsuba 2.0 use "look_at"
		if (element->Value() == std::string("lookAt") || element->Value() == std::string("look_at")) {
			const auto origin = read_vector3(element->Attribute("origin"));
			const auto target = read_vector3(element->Attribute("target"));
			const auto up = read_vector3(element->Attribute("up"));

			transform = rainbow::look_at(origin, target, up).inverse();

			return;
		}

		// todo:
	}
	
	void process_float(const tinyxml2::XMLNode* node, real& value)
	{
		// read real for example :
		// <float name = "float_property" value = "1.">
		value = read_real(node->ToElement()->Attribute("value"));
	}

	template <typename T>
	void process_integer(const tinyxml2::XMLNode* node, T& value)
	{
		// read integer for example :
		// <integer name = "integer_property" value = "1">
		value = read_integer<T>(node->ToElement()->Attribute("value"));
	}
	
	void process_integrator(const tinyxml2::XMLNode* node, scene_info& info)
	{
		// because we only support path integrator, so we do not need process anything
		// we do not create integrator in this function, because we need get the sample_count
	}

	void process_sampler(const tinyxml2::XMLNode* node, scene_info& info) {
		// we do not process the sampler type
		// because the mitsuba 2.0 only support independent sampler now
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_INTEGER_ELEMENT) {
					const std::string name = current->ToElement()->Attribute("name");

					// if the name is "sampleCount"(mitsuba 0.6) or "sample_count"(mitsuba 2.0)
					// we will read the sample per pixel of scene
					if (name == "sampleCount" || name == "sample_count")
						process_integer(current, info.sample_count);
				}
			});
	}

	void process_filter(const tinyxml2::XMLNode* node, std::shared_ptr<filter>& filter)
	{
		const std::string type = node->ToElement()->Attribute("type");

		if (type == "gaussian") filter = std::make_shared<gaussian_filter>();
	}

	void process_film(const tinyxml2::XMLNode* node, scene_info& info)
	{
		const auto crop_window_min = vector2(0.0f, 0.0f);
		const auto crop_window_max = vector2(1.0f, 1.0f);

		const auto crop_window = bound2(
			crop_window_min,
			crop_window_max
		);
		
		std::shared_ptr<filter> filter;
		
		int width = 1280;
		int height = 720;

		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				// when the current is integer value, it may be width or height
				if (current->Value() == MITSUBA_INTEGER_ELEMENT) {
					const std::string name = current->ToElement()->Attribute("name");

					if (name == "height") process_integer(current, height);
					if (name == "width") process_integer(current, width);
				}

				// if we find the filter value, it will be used to create film
				if (current->Value() == MITSUBA_FILTER_ELEMENT)
					process_filter(current, filter);
			});

		info.film = std::make_shared<film>(filter, vector2i(width, height), crop_window);
	}
	
	void process_sensor(const tinyxml2::XMLNode* node, scene_info& info)
	{
		const auto element = node->ToElement();

		// now we only support perspective camera.
		// so if there is no perspective camera, we do not process it
		if (element->Attribute("type") != std::string("perspective")) return;

		transform transform;
		real fov = 45.f;

		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				// if we find the transform element, it should be the transform from camera to world
				// so we process transform of camera
				if (current->Value() == MITSUBA_TRANSFORM_ELEMENT)
					process_transform(current, transform);
			
				if (current->Value() == MITSUBA_SAMPLER_ELEMENT)
					process_sampler(current, info);

				// process the film element and create a film to info
				if (current->Value() == MITSUBA_FILM_ELEMENT)
					process_film(current, info);

				// if we find the float element and its name is fov
				// we will read a float as fov value(angle)
				if (current->Value() == MITSUBA_FLOAT_ELEMENT && current->ToElement()->Attribute("name") == std::string("fov"))
					process_float(current, fov);
			});

		info.camera = std::make_shared<perspective_camera>(info.film, transform, fov);
	}

	void process_scene(const tinyxml2::XMLNode* node, scene_info& info) {
		auto current = node->FirstChild();

		while (current != nullptr) {

			logs::info(current->Value());
			
			if (current->Value() == MITSUBA_INTEGRATOR_ELEMENT)
				process_integrator(current, info);

			if (current->Value() == MITSUBA_SENSOR_ELEMENT)
				process_sensor(current, info);
			
			current = current->NextSibling();
		}
	}

	scene_info loaders::load_mitsuba_scene(const std::string& filename)
	{
		scene_info info;

		tinyxml2::XMLDocument doc;

		doc.LoadFile(filename.c_str());

		process_scene(doc.LastChild(), info);
		
		return info;
	}
	
}

#endif