#include "mitsuba_loader.hpp"

#include "../../integrators/path_integrator.hpp"
#include "../../cameras/perspective_camera.hpp"
#include "../../textures/constant_texture.hpp"
#include "../../materials/matte_material.hpp"
#include "../../filters/gaussian_filter.hpp"
#include "../../samplers/stratified_sampler.hpp"
#include "../../samplers/random_sampler.hpp"
#include "../../emitters/surface_light.hpp"
#include "../../shared/logs/log.hpp"

#include <filesystem>
#include <sstream>

#ifdef __MITSUBA_LOADER__

#define __OBJ_LOADER__

#include "obj_loader.hpp"
#include <tinyxml2.h>

#endif

#ifdef __MITSUBA_LOADER__

const std::string MITSUBA_INTEGRATOR_ELEMENT = "integrator";
const std::string MITSUBA_TRANSLATE_ELEMENT = "translate";
const std::string MITSUBA_TRANSFORM_ELEMENT = "transform";
const std::string MITSUBA_REFERENCE_ELEMENT = "ref";
const std::string MITSUBA_SPECTRUM_ELEMENT = "spectrum";
const std::string MITSUBA_SAMPLER_ELEMENT = "sampler";
const std::string MITSUBA_INTEGER_ELEMENT = "integer";
const std::string MITSUBA_EMITTER_ELEMENT = "emitter";
const std::string MITSUBA_FILTER_ELEMENT = "rfilter";
const std::string MITSUBA_SENSOR_ELEMENT = "sensor";
const std::string MITSUBA_STRING_ELEMENT = "string";
const std::string MITSUBA_SHAPE_ELEMENT = "shape";
const std::string MITSUBA_FLOAT_ELEMENT = "float";
const std::string MITSUBA_FILM_ELEMENT = "film";
const std::string MITSUBA_BSDF_ELEMENT = "bsdf";

namespace rainbow::scenes::loaders {

	void scene_info::build()
	{
		integrator = std::make_shared<path_integrator>(
			std::make_shared<random_sampler2d>(sample_count),
			std::make_shared<random_sampler1d>(sample_count));
		
		scene->build_accelerator();
	}

	void scene_info::render() const
	{
		integrator->render(camera, scene);
	}

	void scene_info::write(const std::string& file_name) const
	{
		film->write(file_name);
	}

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

	spectrum read_spectrum(const std::string& value)
	{
		std::vector<real> lambdas;
		std::vector<real> strengths;
		std::stringstream stream(value);

		std::string wave;
		
		while (std::getline(stream, wave, ',')) {
			const auto middle = wave.find(':');
			const auto lambda = read_real(wave.substr(0, middle));
			const auto strength = read_real(wave.substr(middle + 1, wave.length() - middle - 1));

			lambdas.push_back(lambda);
			strengths.push_back(strength);
		}

		return spectrum(lambdas, strengths);
	}

	void loop_all_children(const tinyxml2::XMLNode* node, const std::function<void(const tinyxml2::XMLNode*)>& function)
	{
		auto current = node->FirstChild();

		while (current != nullptr) {
			function(current);

			current = current->NextSibling();
		}
	}

	void process_look_at(const tinyxml2::XMLNode* node, transform& transform)
	{
		const auto element = node->ToElement();
		
		const auto origin = read_vector3(element->Attribute("origin"));
		const auto target = read_vector3(element->Attribute("target"));
		const auto up = read_vector3(element->Attribute("up"));

		transform *= rainbow::look_at_right_hand(origin, target, up).inverse();
	}

	void process_translate(const tinyxml2::XMLNode* node, transform& transform)
	{
		const auto element = node->ToElement();

		const auto x = read_real(element->Attribute("x"));
		const auto y = read_real(element->Attribute("y"));
		const auto z = read_real(element->Attribute("z"));

		transform *= translate(vector3(x, y, z));
	}
	
	void process_transform(const tinyxml2::XMLNode* node, transform& transform)
	{
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				// "look_at" is mitsuba 2.0 name, "lookAt" is mitsuba 0.6 name
				if (current->Value() == std::string("look_at") || current->Value() == std::string("lookAt"))
					process_look_at(current, transform);

				if (current->Value() == MITSUBA_TRANSLATE_ELEMENT)
					process_translate(current, transform);

				//todo: solve other transform
			});
	}
	
	void process_float(const tinyxml2::XMLNode* node, real& value)
	{
		// read real for example :
		// <float name = "float_property" value = "1."/>
		value = read_real(node->ToElement()->Attribute("value"));
	}

	template <typename T>
	void process_integer(const tinyxml2::XMLNode* node, T& value)
	{
		// read integer for example :
		// <integer name = "integer_property" value = "1"/>
		value = read_integer<T>(node->ToElement()->Attribute("value"));
	}

	void process_spectrum(const tinyxml2::XMLNode* node, spectrum& value)
	{
		// read spectrum
		// <spectrum name = "reflectance" value = "length:value, length:value..."/>
		value = read_spectrum(node->ToElement()->Attribute("value"));
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

		info.camera = std::make_shared<perspective_camera>(info.film, transform, radians(fov));
	}

	void process_diffuse_bsdf(const tinyxml2::XMLNode* node, std::shared_ptr<material>& material)
	{
		auto diffuse = spectrum(1);

		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_SPECTRUM_ELEMENT)
					process_spectrum(current, diffuse);
			});

		material = std::make_shared<matte_material>(
			std::make_shared<constant_texture2d<spectrum>>(diffuse),
			std::make_shared<constant_texture2d<real>>(0.f));
	}
	
	void process_bsdf(const tinyxml2::XMLNode* node, scene_info& info)
	{
		const auto element = node->ToElement();
		const auto type = std::string(element->Attribute("type"));
		const auto id = std::string(element->Attribute("id"));

		std::shared_ptr<material> material;
		
		if (type == "diffuse") process_diffuse_bsdf(node, material);

		info.materials.insert({ id, material });
	}

	void process_obj_mesh(const tinyxml2::XMLNode* node, const scene_info& info, std::shared_ptr<shape>& shape)
	{
		// load obj mesh from value
		// <string name = "filename" value = "path"/>
		shape = load_obj_mesh(info.directory + "/" + node->ToElement()->Attribute("value"))[0];
	}

	void process_reference_bsdf(const tinyxml2::XMLNode* node, const scene_info& info, std::shared_ptr<material>& material)
	{
		material = info.materials.at(node->ToElement()->Attribute("id"));
	}

	void process_emitter_area(const tinyxml2::XMLNode* node, std::shared_ptr<emitter>& emitter)
	{
		auto radiance = spectrum(1);

		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_SPECTRUM_ELEMENT)
					process_spectrum(current, radiance);
			});

		emitter = std::make_shared<surface_light>(radiance);
	}
	
	void process_emitter(const tinyxml2::XMLNode* node, std::shared_ptr<emitter>& emitter)
	{
		const auto type = std::string(node->ToElement()->Attribute("type"));

		if (type == "area") process_emitter_area(node, emitter);
	}
	
	void process_shape(const tinyxml2::XMLNode* node, scene_info& info)
	{
		const auto type = std::string(node->ToElement()->Attribute("type"));

		// we only support obj mode
		if (type != "obj") return;

		std::shared_ptr<material> material;
		std::shared_ptr<emitter> emitter;
		std::shared_ptr<shape> shape;

		transform transform;
		
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_STRING_ELEMENT)
					process_obj_mesh(current, info, shape);

				if (current->Value() == MITSUBA_TRANSFORM_ELEMENT)
					process_transform(current, transform);

				if (current->Value() == MITSUBA_REFERENCE_ELEMENT)
					process_reference_bsdf(current, info, material);

				if (current->Value() == MITSUBA_EMITTER_ELEMENT)
					process_emitter(current, emitter);
			});

		info.scene->add_entity(std::make_shared<entity>(material, emitter, shape, transform));
	}
	
	void process_scene(const tinyxml2::XMLNode* node, scene_info& info) {
		info.scene = std::make_shared<scene>();

		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_INTEGRATOR_ELEMENT)
					process_integrator(current, info);

				if (current->Value() == MITSUBA_SENSOR_ELEMENT)
					process_sensor(current, info);

				if (current->Value() == MITSUBA_BSDF_ELEMENT)
					process_bsdf(current, info);

				if (current->Value() == MITSUBA_SHAPE_ELEMENT)
					process_shape(current, info);
			});
	}

	scene_info loaders::load_mitsuba_scene(const std::string& file_name)
	{
		scene_info info;

		info.directory = std::filesystem::path(file_name).parent_path().generic_string();

		tinyxml2::XMLDocument doc;

		doc.LoadFile(file_name.c_str());

		process_scene(doc.LastChild(), info);

		return info;
	}
	
}

#endif