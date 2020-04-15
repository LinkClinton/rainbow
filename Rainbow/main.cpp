#include "integrators/whitted_integrator.hpp"
#include "cameras/perspective_camera.hpp"
#include "textures/constant_texture.hpp"
#include "materials/plastic_material.hpp"
#include "materials/mirror_material.hpp"
#include "materials/matte_material.hpp"
#include "materials/glass_material.hpp"
#include "samplers/random_sampler.hpp"
#include "filters/box_filter.hpp"
#include "lights/point_light.hpp"
#include "shapes/sphere.hpp"
#include "shapes/disk.hpp"
#include "scenes/scene.hpp"

using namespace rainbow;

vector2 resolution(1280 / 2, 720 / 2);

int main() {
	//const auto crop_window_min = vector2(0.3f);
	//const auto crop_window_max = vector2(0.5f);
	
	const auto crop_window_min = vector2(0.0f, 0.0f);
	const auto crop_window_max = vector2(1.0f, 1.0f);
	
	const auto film = std::make_shared<cameras::film>(
		std::make_shared<box_filter>(vector2(1.f)),
		vector2i(resolution.x, resolution.y),
		bound2(crop_window_min, crop_window_max)
		);
	
	const auto camera = std::make_shared<perspective_camera>(
		film,
		translate(vector3(0, -30, 30)) * rotate(45.f, vector3(1, 0, 0)),
		bound2(
			vector2(-resolution.x * 0.5f, -resolution.y * 0.5f),
			vector2(+resolution.x * 0.5f, +resolution.y * 0.5f)
		),
		quarter_pi<real>()
		);

	const auto scene = std::make_shared<scenes::scene>();

	scene->add_shape(
		std::make_shared<sphere>(
			std::make_shared<glass_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(0.5f)),
				std::make_shared<constant_texture2d<spectrum>>(spectrum(0.5f)),
				std::make_shared<constant_texture2d<vector2>>(vector2(0.0f)),
				std::make_shared<constant_texture2d<real>>(1.f / 1.5f)
				),
			translate(vector3(-11, 0, 10)),
			10.f
			)
	);

	scene->add_shape(
		std::make_shared<sphere>(
			std::make_shared<plastic_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f, 0.f, 0.f)),
				std::make_shared<constant_texture2d<real>>(1.f)
				),
			translate(vector3(11, 0, 10)),
			10.f
			)
	);
	
	scene->add_shape(
		std::make_shared<disk>(
			std::make_shared<plastic_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<spectrum>>(0.83f),
				std::make_shared<constant_texture2d<real>>(0.5f)
				),
			translate(vector3(0, 0, 0.f)),
			40.f
			)
	);
	
	scene->add_light(std::make_shared<point_light>(
		translate(vector3(0, 0, 35)),
		spectrum(400)
		));

	scene->add_light(std::make_shared<point_light>(
		translate(vector3(0, -30, 30)),
		spectrum(400)
		));

	const auto integrator = std::make_shared<integrators::whitted_integrator>(
		std::make_shared<random_sampler2d>(4),
		std::make_shared<random_sampler2d>(0),
		5
		);

	integrator->render(camera, scene);

	film->write("image");
}