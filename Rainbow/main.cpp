#include "integrators/whitted_integrator.hpp"
#include "integrators/path_integrator.hpp"
#include "cameras/perspective_camera.hpp"
#include "textures/constant_texture.hpp"
#include "materials/plastic_material.hpp"
#include "materials/mirror_material.hpp"
#include "materials/matte_material.hpp"
#include "materials/glass_material.hpp"
#include "samplers/stratified_sampler.hpp"
#include "samplers/random_sampler.hpp"
#include "filters/gaussian_filter.hpp"
#include "filters/box_filter.hpp"
#include "emitters/point_light.hpp"
#include "shapes/sphere.hpp"
#include "shapes/disk.hpp"
#include "scenes/scene.hpp"

using namespace rainbow;

vector2 resolution(1280 / 2, 720 / 2);

int main() {
	//const auto crop_window_min = vector2(0.3f);
	//const auto crop_window_max = vector2(0.7f);

	const auto crop_window_min = vector2(0.0f, 0.0f);
	const auto crop_window_max = vector2(1.0f, 1.0f);
	
	const auto film = std::make_shared<cameras::film>(
		std::make_shared<box_filter>(),
		vector2i(resolution.x, resolution.y),
		bound2(crop_window_min, crop_window_max)
		);
	
	const auto camera = std::make_shared<perspective_camera>(
		film,
		translate(vector3(0, -4.0f, 1.5f)) * rotate(90.f, vector3(1, 0, 0)),
		bound2(
			vector2(-resolution.x * 0.5f, -resolution.y * 0.5f),
			vector2(+resolution.x * 0.5f, +resolution.y * 0.5f)
		),
		quarter_pi<real>()
		);

	const auto scene = std::make_shared<scenes::scene>();

	/*scene->add_shape(
		std::make_shared<sphere>(
			std::make_shared<glass_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<vector2>>(vector2(0.f)),
				std::make_shared<constant_texture2d<real>>(1.5f)
				),
			translate(vector3(-1.1f, 0, 1.0f)),
			1.f
			)
	);*/
	
	scene->add_shape(
		std::make_shared<sphere>(
			std::make_shared<plastic_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f, 0.f, 0.f)),
				std::make_shared<constant_texture2d<real>>(0.04f)
				),
			translate(vector3(-1.1f, 0, 1.0f)),
			1.f
			)
	);

	scene->add_shape(
		std::make_shared<sphere>(
			std::make_shared<plastic_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f, 0.f, 0.f)),
				std::make_shared<constant_texture2d<real>>(0.04f)
				),
			translate(vector3(1.1f, 0, 1.0f)),
			1.f
			)
	);
	
	scene->add_shape(
		std::make_shared<disk>(
			std::make_shared<plastic_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<spectrum>>(0.83f),
				std::make_shared<constant_texture2d<real>>(0.3f)
				),
			translate(vector3(0, 0, 0.f)),
			500.f
			)
	);

	scene->add_shape(
		std::make_shared<disk>(
			std::make_shared<plastic_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<real>>(0.4f)
				),
			translate(vector3(0, 1, 0)) * rotate(-90.f, vector3(1, 0, 0)),
			100.f
			)
	);

	scene->add_emitter(std::make_shared<point_light>(
		translate(vector3(0, 0, 5.0)),
		spectrum(4)
		));

	scene->add_emitter(std::make_shared<point_light>(
		translate(vector3(0, -3.0, 5.0)),
		spectrum(4)
		));

	const auto samples_per_pixel_x = static_cast<size_t>(4);
	const auto samples_per_pixel_y = static_cast<size_t>(4);
	const auto samples_per_pixel = samples_per_pixel_x * samples_per_pixel_y;
	const auto dimension = 8;
	
	/*const auto integrator = std::make_shared<integrators::whitted_integrator>(
		std::make_shared<random_sampler2d>(samples_per_pixel),
		5
		);*/
	
	const auto integrator = std::make_shared<integrators::path_integrator>(
		//std::make_shared<random_sampler2d>(samples_per_pixel),
		//std::make_shared<random_sampler1d>(samples_per_pixel),
		std::make_shared<stratified_sampler2d>(samples_per_pixel_x, samples_per_pixel_y, dimension),
		std::make_shared<stratified_sampler1d>(samples_per_pixel_x, samples_per_pixel_y, dimension),
		5
		);

	integrator->render(camera, scene);
	
	film->write("image");
}