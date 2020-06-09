#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/spectrums/spectrum.hpp"
#include "../samplers/samplers.hpp"
#include "../cameras/camera.hpp"
#include "../scenes/scene.hpp"

namespace rainbow::cpus::integrators {

	using namespace shared::spectrums;
	using namespace samplers;
	using namespace cameras;
	using namespace scenes;

	struct integrator_debug_info {
		vector2i pixel = vector2i(0);
		size_t sample = 0;

		integrator_debug_info() = default;

		integrator_debug_info(const vector2i& pixel, size_t sample);
	};

	struct sampler_group {
		std::shared_ptr<sampler1d> sampler1d;
		std::shared_ptr<sampler2d> sampler2d;

		sampler_group() = default;

		sampler_group(
			const std::shared_ptr<samplers::sampler1d>& sampler1d,
			const std::shared_ptr<samplers::sampler2d>& sampler2d);

		void next_sample() const noexcept;

		void reset() const noexcept;
	};

	struct path_tracing_info {
		spectrum value = spectrum(0);
		spectrum beta = spectrum(1);

		bool specular = false;
		
		real eta = 1;

		ray ray;

		path_tracing_info() = default;

		path_tracing_info(
			const spectrum& value, const spectrum& beta,
			const shared::ray& ray, real eta, bool specular);
	};

	class integrator : public interfaces::noncopyable {
	public:
		integrator() = default;

		~integrator() = default;

		virtual void render(
			const std::shared_ptr<camera>& camera,
			const std::shared_ptr<scene>& scene) = 0;

		void set_debug_trace_pixel(const vector2i& pixel);
	protected:
		std::vector<vector2i> mDebugPixels;
	};

	std::tuple<std::optional<surface_interaction>, real> find_emitter(
		const std::shared_ptr<scene>& scene, const sampler_group& samplers,
		const interaction& interaction, const vector3& wi);

	std::tuple<std::shared_ptr<const entity>, real> uniform_sample_one_emitter(
		const std::shared_ptr<scene>& scene, const sampler_group& samplers);

	spectrum uniform_sample_one_emitter(
		const std::shared_ptr<scene>& scene, const sampler_group& samplers,
		const surface_interaction& interaction, const scattering_function_collection& functions, 
		bool media);

	spectrum uniform_sample_one_emitter(
		const std::shared_ptr<scene>& scene, const sampler_group& samplers,
		const medium_interaction& interaction);

	bool sample_scattering_surface_function(
		const std::shared_ptr<scene>& scene, const sampler_group& samplers,
		const surface_properties& properties, path_tracing_info& tracing_info, bool media);

	bool sample_surface_interaction(
		const std::shared_ptr<scene>& scene, const sampler_group& samplers,
		const std::optional<surface_interaction>& interaction, 
		path_tracing_info& tracing_info, int& bounces, bool media);

	bool sample_medium_interaction(
		const std::shared_ptr<scene>& scene, const sampler_group& samplers,
		const std::optional<medium_interaction>& interaction,
		path_tracing_info& tracing_info, int& bounces);
	
	real power_heuristic(real f_pdf, real g_pdf);
}