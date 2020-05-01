#include "../integrators/whitted_integrator.hpp"
#include "../integrators/path_integrator.hpp"
#include "../cameras/perspective_camera.hpp"
#include "../textures/constant_texture.hpp"
#include "../textures/image_texture.hpp"
#include "../materials/plastic_material.hpp"
#include "../materials/mirror_material.hpp"
#include "../materials/matte_material.hpp"
#include "../materials/glass_material.hpp"
#include "../samplers/stratified_sampler.hpp"
#include "../samplers/random_sampler.hpp"
#include "../filters/gaussian_filter.hpp"
#include "../filters/box_filter.hpp"
#include "../emitters/environment_light.hpp"
#include "../emitters/surface_light.hpp"
#include "../emitters/point_light.hpp"
#include "../shared/file_system.hpp"
#include "../shapes/sphere.hpp"
#include "../shapes/disk.hpp"
#include "../shapes/mesh.hpp"
#include "../scenes/scene.hpp"

#include "../shared/distributions/distribution.hpp"

using namespace rainbow;

int main() {
	
	const vector2 resolution(1280 / 2, 720 / 2);

	//const auto crop_window_min = vector2(0.2f, 0.55f);
	//const auto crop_window_max = vector2(0.4f, 0.75f);

	const auto crop_window_min = vector2(0.0f, 0.0f);
	const auto crop_window_max = vector2(1.0f, 1.0f);
	
	const auto film = std::make_shared<cameras::film>(
		std::make_shared<box_filter>(),
		vector2i(resolution.x, resolution.y),
		bound2(crop_window_min, crop_window_max)
		);
	
	const auto camera = std::make_shared<perspective_camera>(
		film,
		translate(vector3(0, -10, 0)) * rotate(90.f, vector3(1, 0, 0)),
		bound2(
			vector2(-resolution.x, -resolution.y) * 0.28f,
			vector2(+resolution.x, +resolution.y) * 0.28f
		),
		quarter_pi<real>()
		);

	const auto scene = std::make_shared<scenes::scene>();

	scene->add_entity(
		std::make_shared<entity>(
			std::make_shared<plastic_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<real>>(0.f)),
			nullptr,
			std::make_shared<sphere>(2.f),
			translate(vector3(0, 0, 0.f)))
	);

	/*scene->add_entity(
		std::make_shared<entity>(
			std::make_shared<plastic_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<spectrum>>(0.83f),
				std::make_shared<constant_texture2d<real>>(0.3f)),
			nullptr,
			mesh::create_quad(100, 100),
			translate(vector3(0, 0, 0)))
	);

	scene->add_entity(
		std::make_shared<entity>(
			std::make_shared<plastic_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f)),
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f, 0, 0)),
				std::make_shared<constant_texture2d<real>>(0.3f)),
			nullptr,
			mesh::create_quad(100, 100),
			translate(vector3(0, 2.f, 0)) * rotate(90.f, vector3(-1, 0, 0)))
	);*/
	
	/*scene->add_entity(
		std::make_shared<entity>(nullptr,
			std::make_shared<surface_light>(spectrum(4)),
			mesh::create_quad(4, 4),
			translate(vector3(0, 0.f, 9.f)) * rotate(180.f, vector3(1, 0, 0)))
	);*/

	scene->add_entity(
		std::make_shared<entity>(nullptr,
			std::make_shared<environment_light>(
				file_system::read_texture2d_hdr<spectrum>("./any.hdr"),
				spectrum(1.f), 20.f),
			nullptr,
			scale(vector3(-1, 1, 1)))
	);
	
	const auto samples_per_pixel_x = static_cast<size_t>(4);
	const auto samples_per_pixel_y = static_cast<size_t>(4);
	const auto samples_per_pixel = samples_per_pixel_x * samples_per_pixel_y;
	const auto dimension = 16;
	
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

	integrator->set_debug_trace_pixel(vector2i(277, 165));
	
	integrator->render(camera, scene);
	
	film->write("image");

	system("pause");
}