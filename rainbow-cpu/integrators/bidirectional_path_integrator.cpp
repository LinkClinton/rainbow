#include "bidirectional_path_integrator.hpp"

#include <variant>

namespace rainbow::cpus::integrators {

	inline real correct_shading_normal(
		const surface_interaction& interaction, 
		const vector3& wo, const vector3& wi, 
		const transport_mode& mode)
	{
		if (mode == transport_mode::radiance) return 1;

		const auto numerator = 
			math::abs(dot(wo, interaction.shading_space.z())) * 
			math::abs(dot(wi, interaction.normal));
		const auto denominator =
			math::abs(dot(wo, interaction.normal)) *
			math::abs(dot(wi, interaction.shading_space.z()));

		if (denominator == 0) return 0;

		return numerator / denominator;
	}
	
	enum class vertex_type : uint32 {
		unknown = 0,
		surface = 1,
		medium = 2,
		camera = 3,
		emitter = 4
	};

	struct point_interaction final : interaction {
		std::variant<std::shared_ptr<camera>, std::shared_ptr<const entity>> which;

		point_interaction() = default;

		point_interaction(const std::variant<std::shared_ptr<camera>, std::shared_ptr<const entity>>& which) : which(which) {}

		point_interaction(const interaction& interaction,
			const std::variant<std::shared_ptr<camera>, std::shared_ptr<const entity>>& which) : interaction(interaction), which(which) {}

		std::shared_ptr<const entity> emitter() const noexcept { return std::get<std::shared_ptr<const entity>>(which); }

		std::shared_ptr<camera> camera() const noexcept { return std::get<std::shared_ptr<cameras::camera>>(which); }
	};

	struct vertex {
		std::variant<surface_interaction, medium_interaction, point_interaction> which;

		surface_properties properties;
		
		vertex_type type = vertex_type::unknown;

		spectrum beta = spectrum(0);

		real forward_pdf = 0;
		real reverse_pdf = 0;

		bool delta = false;

		vertex() = default;

		vertex(
			const std::variant<surface_interaction, medium_interaction, point_interaction>& which,
			const surface_properties& properties, const vertex_type& type, const spectrum& beta, 
			real forward_pdf, real reverse_pdf, bool delta) :
			which(which), properties(properties), type(type), beta(beta),
			forward_pdf(forward_pdf), reverse_pdf(reverse_pdf), delta(delta)
		{
		}

		const interaction& interaction() const
		{
			if (type == vertex_type::surface) return std::get<surface_interaction>(which);
			if (type == vertex_type::medium) return std::get<medium_interaction>(which);

			return std::get<point_interaction>(which);
		}

		std::shared_ptr<const entity> emitter() const
		{
			if (type == vertex_type::emitter) return std::get<point_interaction>(which).emitter();

			if (type == vertex_type::surface) {
				const auto entity = std::get<surface_interaction>(which).entity;

				if (entity->has_component<emitters::emitter>()) return entity;

				return nullptr;
			}
					
			return nullptr;
		}

		spectrum evaluate(const vertex& next, const transport_mode& mode) const
		{
			if (type != vertex_type::surface && type != vertex_type::medium)
				return spectrum(0);
			
			auto world_wi = next.interaction().point - interaction().point;

			if (length_squared(world_wi) == 0) return spectrum(0);

			world_wi = normalize(world_wi);
			
			if (type == vertex_type::surface) {
				const auto& surface = std::get<surface_interaction>(which);
				
				const auto wo = world_to_local(surface.shading_space, surface.wo);
				const auto wi = world_to_local(surface.shading_space, world_wi);

				return properties.functions.evaluate(wo, wi) * correct_shading_normal(surface, surface.wo, world_wi, mode);
			}

			const auto& medium = std::get<medium_interaction>(which);

			return spectrum(medium.function->evaluate(medium.wo, world_wi));
		}

		spectrum evaluate(const std::shared_ptr<scene>& scene, const vertex& point) const
		{
			const auto emitter = this->emitter();

			// if the emitter is nullptr and type = emitter it should intersect with environment emitter
			// but the environment emitter is empty in scene, so the emitter is nullptr, we can just return 0
			// if the emitter is nullptr and type = surface, means the entity of surface does not have emitter
			// in other types, the emitter must be nullptr
			if (emitter == nullptr) return spectrum(0);
			
			auto w = point.interaction().point - interaction().point;

			if (length_squared(w) == 0) return spectrum(0);

			w = normalize(w);

			if (emitter->component<emitters::emitter>()->is_environment()) {
				auto L = spectrum(0);

				for (const auto& environment : scene->environments())
					L += environment->evaluate<emitters::emitter>(interaction(), w);

				return L;
			}

			return emitter->component<emitters::emitter>()->evaluate(interaction(), w);
		}

		real pdf(const std::shared_ptr<scene>& scene, const vertex& last, const vertex& next) const
		{
			if (type == vertex_type::emitter) return pdf(scene, next);

			if (type == vertex_type::camera) {
				auto w = next.interaction().point - interaction().point;

				if (length_squared(w) == 0) return 0;

				w = normalize(w);

				const auto interaction = std::get<point_interaction>(which);
				const auto [pdf_position, pdf_direction] = interaction.camera()->pdf(interaction.spawn_ray(w));
				
				return convert_density(pdf_direction, next);
			}

			const auto next_w = next.interaction().point - interaction().point;
			const auto last_w = last.interaction().point - interaction().point;

			if (length_squared(next_w) == 0 || length_squared(last_w) == 0) return 0;

			real pdf = 0;

			if (type == vertex_type::surface) {
				const auto interaction = std::get<surface_interaction>(which);
				const auto wo = world_to_local(interaction.shading_space, normalize(last_w));
				const auto wi = world_to_local(interaction.shading_space, normalize(next_w));

				pdf = properties.functions.pdf(wo, wi);
			}

			if (type == vertex_type::medium) {
				const auto interaction = std::get<medium_interaction>(which);

				pdf = interaction.function->evaluate(last_w, next_w);
			}

			return convert_density(pdf, next);
		}

		real pdf(const std::shared_ptr<scene>& scene, const vertex& next) const
		{
			if (is_environment()) {
				const auto [center, radius] = scene->bounding_sphere();
				const auto w = normalize(next.interaction().point - interaction().point);
				
				auto pdf = 1 / (pi<real>() * radius * radius);

				// abs(dot(normal, w)) == abs(dot(normal, -w))
				if (next.on_surface()) pdf = pdf * math::abs(dot(next.interaction().normal, w));

				return pdf;
			}

			const auto inv_distance_2 = distance_squared(next.interaction().point, interaction().point);
			const auto w = normalize(next.interaction().point - interaction().point);

			const auto emitter = this->emitter();

			const auto [pdf_position, pdf_direction] = emitter->pdf<emitters::emitter>(
				ray(w, interaction().point), interaction().normal);

			auto pdf = pdf_direction * inv_distance_2;

			// abs(dot(normal, w)) == abs(dot(normal, -w))
			if (next.on_surface()) pdf = pdf * math::abs(dot(next.interaction().normal, w));

			return pdf;
		}
		
		real convert_density(real pdf, const vertex& next) const
		{
			if (next.is_environment()) return pdf;

			const auto w = next.interaction().point - interaction().point;

			if (length_squared(w) == 0) return 0;

			const auto inv_distance_2 = 1 / length_squared(w);

			if (next.on_surface()) 
				pdf = pdf * math::abs(dot(next.interaction().normal, normalize(w)));

			return pdf * inv_distance_2;
		}

		bool is_environment() const
		{
			if (type != vertex_type::emitter) return false;

			const auto emitter = std::get<point_interaction>(which).emitter();

			return emitter == nullptr || emitter->component<emitters::emitter>()->is_environment();
		}

		bool on_surface() const
		{
			return interaction().normal != vector3(0);
		}
		
		bool connectible() const {
			if (type == vertex_type::surface) 
				return properties.functions.count(scattering_type::all ^ scattering_type::specular) > 0;

			if (type == vertex_type::medium)
				return true;

			if (type == vertex_type::camera)
				return true;

			const auto emitter = std::get<point_interaction>(which).emitter();

			return !has(emitter->component<emitters::emitter>()->type(), emitter_type::delta_direction);
		}
	};

	inline vertex create_camera_vertex(const std::shared_ptr<camera>& camera, const ray& ray, const spectrum& beta)
	{
		return vertex(
			point_interaction(interaction(ray.origin), camera),
			surface_properties(),
			vertex_type::camera,
			beta,
			0, 0, false);
	}

	inline vertex create_emitter_vertex(const std::shared_ptr<const entity>& emitter, const emitter_ray_sample& ray_sample)
	{
		return vertex(
			point_interaction(interaction(ray_sample.normal, ray_sample.ray.origin, ray_sample.ray.direction), emitter),
			surface_properties(),
			vertex_type::emitter,
			ray_sample.intensity,
			ray_sample.pdf_direction * ray_sample.pdf_position,
			0, false);
	}
	
	inline std::vector<vertex> generate_camera_sub_path(
		const std::shared_ptr<camera>& camera,
		const std::shared_ptr<scene>& scene,
		const sampler_group& samplers,
		const vector2& position,
		size_t max_depth)
	{
		if (max_depth == 0) return {};
		
		const auto ray = camera->sample(position, samplers.sampler2d->next());
		const auto beta = spectrum(1);
		
		std::vector<vertex> vertices;

		vertices.push_back(create_camera_vertex(camera, ray, beta));

		// todo : build the sub path
	}

	inline std::vector<vertex> generate_emitter_sub_path(
		const std::shared_ptr<scene>& scene,
		const sampler_group& samplers,
		size_t max_depth)
	{
		if (max_depth == 0) return {};

		const auto [emitter, pdf] = uniform_sample_one_emitter(scene, samplers);

		const auto ray_sample = emitter->sample<emitters::emitter>(samplers.sampler2d->next(), samplers.sampler2d->next());

		if (ray_sample.intensity.is_black() || ray_sample.pdf_position == 0 || ray_sample.pdf_direction == 0) return {};

		std::vector<vertex> vertices;

		vertices.push_back(create_emitter_vertex(emitter, ray_sample));

		// todo : build the sub path
	}
}

rainbow::cpus::integrators::bidirectional_path_integrator::bidirectional_path_integrator(
	const std::shared_ptr<sampler2d>& sampler2d, 
	const std::shared_ptr<sampler1d>& sampler1d,
	size_t max_depth) :
	mSampler2D(sampler2d), mSampler1D(sampler1d), mMaxDepth(max_depth)
{
}