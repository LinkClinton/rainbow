#include "integrators/whitted_integrator.hpp"
#include "cameras/perspective_camera.hpp"
#include "textures/constant_texture.hpp"
#include "materials/matte_material.hpp"
#include "samplers/random_sampler.hpp"
#include "filters/box_filter.hpp"
#include "lights/point_light.hpp"
#include "shapes/sphere.hpp"
#include "scenes/scene.hpp"

using namespace rainbow;

vector2 resolution(1280 / 2, 720 / 2);

int main() {
	const auto film = std::make_shared<cameras::film>(
		std::make_shared<box_filter>(vector2(1.f)),
		vector2i(resolution.x, resolution.y),
		bound2(vector2(0.3f), vector2(0.7f))
		);
	
	const auto camera = std::make_shared<perspective_camera>(
		film,
		transform(),
		bound2(
			vector2(-resolution.x * 0.5f, -resolution.y * 0.5f),
			vector2(+resolution.x * 0.5f, +resolution.y * 0.5f)
		),
		quarter_pi<real>()
		);

	const auto scene = std::make_shared<scenes::scene>();

	scene->add_shape(
		std::make_shared<sphere>(
			std::make_shared<matte_material>(
				std::make_shared<constant_texture2d<spectrum>>(spectrum(1.f, 0.f, 0.f)),
				std::make_shared<constant_texture2d<real>>(0.f)
				),
			translate(vector3(0, 0, -20.f)),
			5.f
			)
	);

	scene->add_light(std::make_shared<point_light>(
		translate(vector3(0, 20, 0)),
		spectrum(1000)
		));

	scene->add_light(std::make_shared<point_light>(
		translate(vector3(0, 0, 0)),
		spectrum(300)
		));

	const auto integrator = std::make_shared<integrators::whitted_integrator>(
		std::make_shared<random_sampler2d>(4),
		std::make_shared<random_sampler2d>(64),
		5
		);

	integrator->render(camera, scene);

	film->write("image");
}