#include "bidirectional_path_integrator.hpp"

#include "../../rainbow-core/logs/log.hpp"
#include "../shared/scope_assignment.hpp"

#pragma optimize("", off)

#ifndef _DEBUG
#define __PARALLEL_RENDER__
#endif

#include <execution>
#include <variant>
#include <chrono>
#include <set>

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
		medium_info medium;
		
		vertex_type type = vertex_type::unknown;

		spectrum beta = spectrum(0);

		real forward_pdf = 0;
		real reverse_pdf = 0;

		bool delta = false;

		vertex() = default;

		vertex(
			const std::variant<surface_interaction, medium_interaction, point_interaction>& which,
			const surface_properties& properties, const medium_info& medium, 
			const vertex_type& type, const spectrum& beta, 
			real forward_pdf, real reverse_pdf, bool delta) :
			which(which), properties(properties), medium(medium), type(type), beta(beta),
			forward_pdf(forward_pdf), reverse_pdf(reverse_pdf), delta(delta)
		{
			// which is a std::variant handle the interaction depend on the type of vertex
			// vertex_type::surface handle a surface_interaction and a surface_properties
			// vertex_type::medium handle a medium_interaction
			// vertex_type::camera handle a point_interaction with pointer of camera
			// vertex_type::emitter handle a point_interaction with pointer of emitter

			// properties is the surface properties of this vertex
			// medium is the medium info
			// for vertex_type::camera, it is empty(we think the space the ray from camera is vacuum)
			// for vertex_type::medium, it is the current medium of it
			// for vertex_type::surface and vertex_type::emitter
			// it is the medium from last vertex to this vertex if the vertex does not have media component
			
			// beta is the value from the start of vertex to this vertex
			// forward_pdf is the pdf from last vertex to this vertex
			// reverse_pdf is the pdf from next vertex to this vertex
		}

		const interaction& interaction() const
		{
			if (type == vertex_type::surface) return std::get<surface_interaction>(which);
			if (type == vertex_type::medium) return std::get<medium_interaction>(which);

			return std::get<point_interaction>(which);
		}
		
		vector3 shading_normal() const
		{
			// return the shading normal, if the type is vertex_type::surface we will use shading_space.z()
			// otherwise, we will use the normal of interaction
			if (type == vertex_type::surface)
				return std::get<surface_interaction>(which).shading_space.z();

			return interaction().normal;
		}

		std::shared_ptr<const entity> emitter() const
		{
			// return the emitter if the vertex has emitter
			// if the type is vertex_type::emitter, we just return the point_interaction::emitter()
			// if point_interaction::emitter() is nullptr, means there is no environment emitter
			// if the type is vertex_type::surface, we will return the surface_interaction::entity(if the entity has emitter)

			// type is vertex_type::emitter
			if (type == vertex_type::emitter) return std::get<point_interaction>(which).emitter();

			// type is vertex_type::surface, if the surface_interaction::entity has emitter, we will return this entity
			if (type == vertex_type::surface) {
				const auto entity = std::get<surface_interaction>(which).entity;

				if (entity->has_component<emitters::emitter>()) return entity;

				return nullptr;
			}

			// if the type is vertex_type::medium or vertex_type::camera we will return nullptr
			return nullptr;
		}

		spectrum evaluate_media_beam(
			const std::shared_ptr<scene>& scene, const sampler_group& samplers,
			const interactions::interaction& to) const
		{
			// evaluate the media beam from this vertex to point
			
			// in fact, the type of vertex is impossible to be emitter
			// so we do not discuss this type
			assert(type != vertex_type::vertex_type::emitter);
			
			if (type == vertex_type::camera)
				return scene->evaluate_media_beam(samplers.sampler1d, { medium_info(), interaction() }, to);

			// for medium, vertex::medium is the medium the point in
			// so we just use medium to evaluate value
			if (type == vertex_type::medium)
				return scene->evaluate_media_beam(samplers.sampler1d, { medium, interaction() }, to);

			// for surface, if the entity surface on does not have media
			// we will use the vertex::medium(the medium from last vertex to this vertex)
			// because it means there is no different between two side of surface
			// otherwise, we will create a new medium_info 
			const auto interaction = std::get<surface_interaction>(which);

			const auto medium =
				interaction.entity->has_component<cpus::media::media>() ?
				medium_info(interaction.entity, interaction.normal, normalize(to.point - interaction.point)) :
				this->medium;

			return scene->evaluate_media_beam(samplers.sampler1d, { medium, interaction }, to);
		}

		spectrum evaluate(const vertex& next, const transport_mode& mode) const
		{
			// evaluate the value between two vertex(this vertex and next vertex)
			// only support vertex_type::surface and vertex_type::medium
			// the mode indicate the direction of transporting(from camera or from emitter)
			
			if (type != vertex_type::surface && type != vertex_type::medium)
				return spectrum(0);
			
			auto world_wi = next.interaction().point - interaction().point;

			if (length_squared(world_wi) == 0) return spectrum(0);

			world_wi = normalize(world_wi);
			
			if (type == vertex_type::surface) {
				const auto& surface = std::get<surface_interaction>(which);

				// transform the wo and wi from world space to local space
				const auto wo = world_to_local(surface.shading_space, surface.wo);
				const auto wi = world_to_local(surface.shading_space, world_wi);

				return properties.functions.evaluate(wo, wi) * correct_shading_normal(surface, surface.wo, world_wi, mode);
			}

			const auto& medium = std::get<medium_interaction>(which);

			return spectrum(medium.function->evaluate(medium.wo, world_wi));
		}

		spectrum evaluate(const std::shared_ptr<scene>& scene, const vertex& point) const
		{
			// evaluate the value from emitter to point vertex(this vertex should be the emitter vertex)
			// we do not consider the volume scattering(we will solve it in other function)
			
			const auto emitter = this->emitter();

			// if the emitter is nullptr and type = emitter it should intersect with environment emitter
			// but the environment emitter is empty in scene, so the emitter is nullptr, we can just return 0
			// if the emitter is nullptr and type = surface, means the entity of surface does not have emitter
			// in other types, the emitter must be nullptr
			if (emitter == nullptr) return spectrum(0);
			
			auto w = point.interaction().point - interaction().point;

			if (length_squared(w) == 0) return spectrum(0);

			w = normalize(w);

			// if the emitter is environment emitter, we will evaluate the all environment emitter
			// in fact, we will consider the one emitter that contains all environment emitters
			if (emitter->component<emitters::emitter>()->is_environment()) {
				auto L = spectrum(0);

				for (const auto& environment : scene->environments())
					L += environment->evaluate<emitters::emitter>(interaction(), w);

				return L;
			}

			return emitter->component<emitters::emitter>()->evaluate(interaction(), w);
		}

		real pdf(const vertex& next) const
		{
			// evaluate the density pdf from this vertex to next vertex
			// the function is used for vertex_type::camera
			// we evaluate the pdf_direction and convert it to density pdf

			assert(type == vertex_type::camera);

			auto w = next.interaction().point - interaction().point;

			if (length_squared(w) == 0) return 0;

			w = normalize(w);

			const auto interaction = std::get<point_interaction>(which);
			const auto [pdf_position, pdf_direction] = interaction.camera()->pdf(ray(w, interaction.point));

			return convert_density(pdf_direction, next);
		}

		real pdf(const vertex& last, const vertex& next) const
		{
			// evaluate the density pdf from this vertex to next vertex
			// the function is used for vertex_type::surface or vertex_type::medium
			// we evaluate properties.functions.pdf or the value of phase(the value and pdf of phase are same)
			// and convert it from solid angle pdf to density pdf

			assert(type == vertex_type::surface || type == vertex_type::medium);
			
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

				pdf = interaction.function->evaluate(normalize(last_w), normalize(next_w));
			}

			return convert_density(pdf, next);
		}
		
		real pdf(const std::shared_ptr<scene>& scene, const vertex& next) const
		{
			// evaluate the density pdf from this vertex to next vertex
			// the function is used for vertex_type::emitter
			
			if (is_environment_emitter()) {
				const auto [center, radius] = scene->bounding_sphere();
				const auto w = normalize(next.interaction().point - interaction().point);
				
				auto pdf = 1 / (pi<real>() * radius * radius);

				// abs(dot(normal, w)) == abs(dot(normal, -w))
				if (next.on_surface()) pdf = pdf * math::abs(dot(next.interaction().normal, w));

				return pdf;
			}

			const auto inv_distance_2 = 1 / distance_squared(next.interaction().point, interaction().point);
			const auto w = normalize(next.interaction().point - interaction().point);

			const auto emitter = this->emitter();

			const auto [pdf_position, pdf_direction] = emitter->pdf<emitters::emitter>(
				ray(w, interaction().point), interaction().normal);

			// convert it from solid angle pdf to density pdf
			auto pdf = pdf_direction * inv_distance_2;

			// abs(dot(normal, w)) == abs(dot(normal, -w))
			if (next.on_surface()) pdf = pdf * math::abs(dot(next.interaction().normal, w));

			return pdf;
		}

		real pdf_emitter_environment(const std::shared_ptr<scene>& scene, const vector3& w) const
		{
			real pdf = 0;
			
			for (const auto& environment : scene->environments())
				pdf = pdf + environment->pdf<emitters::emitter>(interaction(), -w);

			return pdf / scene->emitters().size();
		}
		
		real pdf_emitter_origin(const std::shared_ptr<scene>& scene, const vertex& last) const
		{
			// last is the point need be shading, so the w is the direction from emitter to point
			auto w = last.interaction().point - interaction().point;

			if (length_squared(w) == 0) return 0;

			w = normalize(w);

			if (is_environment_emitter()) return pdf_emitter_environment(scene, w);

			const auto emitter = this->emitter();

			const auto [pdf_position, pdf_direction] = emitter->pdf<emitters::emitter>(
				ray(w, interaction().point), interaction().normal);

			return pdf_position / scene->emitters().size();
		}
		
		real convert_density(real pdf, const vertex& next) const
		{
			if (next.is_environment_emitter()) return pdf;

			const auto w = next.interaction().point - interaction().point;

			if (length_squared(w) == 0) return 0;

			const auto inv_distance_2 = 1 / length_squared(w);

			if (next.on_surface()) 
				pdf = pdf * math::abs(dot(next.interaction().normal, normalize(w)));

			return pdf * inv_distance_2;
		}

		bool is_environment_emitter() const
		{
			if (type != vertex_type::emitter) return false;

			const auto emitter = std::get<point_interaction>(which).emitter();

			return emitter == nullptr || emitter->component<emitters::emitter>()->is_environment();
		}

		bool is_delta_emitter() const
		{
			if (type != vertex_type::emitter) return false;

			const auto emitter = std::get<point_interaction>(which).emitter();

			return emitter != nullptr && emitter->component<emitters::emitter>()->is_delta();
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

	inline vertex create_camera_vertex(const std::shared_ptr<camera>& camera, const spectrum& beta, const ray& ray)
	{
		return vertex(
			point_interaction(interaction(ray.origin), camera),
			surface_properties(), medium_info(),
			vertex_type::camera,
			beta,
			0, 0, false);
	}

	inline vertex create_camera_vertex(const std::shared_ptr<camera>& camera, const interaction& interaction, const spectrum& beta)
	{
		return vertex(
			point_interaction(interaction, camera),
			surface_properties(), medium_info(),
			vertex_type::camera,
			beta, 0, 0, false);
	}

	inline vertex create_emitter_vertex(const std::shared_ptr<const entity>& emitter, const emitter_ray_sample& ray_sample, const medium_info& medium)
	{
		return vertex(
			point_interaction(interaction(ray_sample.normal, ray_sample.ray.origin, ray_sample.ray.direction), emitter),
			surface_properties(), medium,
			vertex_type::emitter,
			ray_sample.intensity,
			ray_sample.pdf_direction * ray_sample.pdf_position,
			0, false);
	}

	inline vertex create_emitter_vertex(const std::shared_ptr<const entity>& emitter, const spectrum& beta, const ray& ray, real pdf)
	{
		return vertex(
			point_interaction(interaction(-ray.direction, ray.origin + ray.direction, ray.direction), emitter),
			surface_properties(), medium_info(),
			vertex_type::emitter,
			beta, pdf, 0, false
		);
	}

	inline vertex create_emitter_vertex(const std::shared_ptr<const entity>& emitter, const interaction& interaction, const spectrum& beta, real pdf)
	{
		return vertex(
			point_interaction(interaction, emitter),
			surface_properties(), medium_info(),
			vertex_type::emitter,
			beta, pdf, 0, false);
	}

	inline vertex create_surface_vertex(
		const surface_interaction& interaction, const surface_properties& properties,
		const medium_info& medium, const spectrum& beta, const vertex& last, real pdf)
	{
		auto v = vertex(interaction, properties, medium, vertex_type::surface, beta, 0, 0, false);

		v.forward_pdf = last.convert_density(pdf, v);

		return v;
	}

	inline vertex create_medium_vertex(
		const medium_interaction& interaction, const medium_info& medium, 
		const spectrum& beta, const vertex& last, real pdf)
	{
		auto v = vertex(interaction, surface_properties(), medium, vertex_type::medium, beta, 0, 0, false);

		v.forward_pdf = last.convert_density(pdf, v);

		return v;
	}

	inline void generate_sub_path(
		const std::shared_ptr<scene>& scene, const sampler_group& samplers,
		const transport_mode& mode, const medium_info& medium, 
		const spectrum& beta, const ray& ray, real pdf, 
		size_t max_depth, std::vector<vertex>& vertices)
	{
		// generate the sub path from emitter or camera the transport_mode will indicate the direction of path
		// the vertices[0] will be the start of path(camera or emitter), the creation is not included in this function
		
		if (max_depth == 0) return;

		path_tracing_info tracing_info;

		// the tracing_info.beta is the value of beta in current vertex(from the start vertex to this vertex)
		// the tracing_info.ray is the ray from last vertex to current vertex
		tracing_info.beta = beta;
		tracing_info.ray = ray;
		tracing_info.medium = medium;

		// the forward_pdf means the pdf from last vertex to this vertex
		// the reverse_pdf means the pdf from next vertex to this vertex
		real forward_pdf = pdf, reverse_pdf = 0;

		for (auto bounces = 0; bounces < max_depth; bounces++) {
			const auto interaction = scene->intersect(tracing_info.ray);

			// sample the medium
			const auto medium_sample = tracing_info.medium.sample(samplers.sampler1d, tracing_info.ray);

			tracing_info.beta *= medium_sample.value;

			if (tracing_info.beta.is_black()) break;

			// if the medium_sample.interaction has value, we will sample the medium's phase function and build new ray
			// otherwise, we will sample surface_interaction
			if (medium_sample.interaction.has_value()) {
				vertices.push_back(create_medium_vertex(medium_sample.interaction.value(), tracing_info.medium, tracing_info.beta,
					vertices.back(), forward_pdf));

				const auto phase_sample = medium_sample.interaction->function->sample(
					medium_sample.interaction.value(), samplers.sampler2d->next());

				forward_pdf = phase_sample.value;
				reverse_pdf = phase_sample.value;

				tracing_info.ray = medium_sample.interaction->spawn_ray(phase_sample.wi);

				auto& last = vertices[vertices.size() - 2];

				last.reverse_pdf = vertices.back().convert_density(reverse_pdf, last);
				
				continue;
			}
			
			if (!interaction.has_value()) {

				// the mode is transport_mode::radiance means the path start from camera
				// so if the ray is not intersect with anything, we can think it intersect environment emitters
				if (mode == transport_mode::radiance) {
					// get the environment emitters, if the environments is empty in scene, we set it into nullptr
					// otherwise, we will use the first environment emitter
					const auto emitter = scene->environments().empty() ? nullptr : scene->environments()[0];

					vertices.push_back(create_emitter_vertex(emitter, tracing_info.beta, tracing_info.ray, forward_pdf));

					vertices.back().medium = tracing_info.medium;
				}

				break;
			}

			if (!interaction->entity->has_component<material>()) {
				// compute the new ray 
				tracing_info.ray = interaction->spawn_ray(tracing_info.ray.direction);

				// update the medium property when interaction->entity has media
				// if interaction->normal dot ray.direction > 0, the medium we tracing should be outside of entity
				// otherwise the medium should be inside
				if (interaction->entity->has_component<cpus::media::media>())
					tracing_info.medium = medium_info(interaction->entity, interaction->normal, tracing_info.ray.direction);
				
				// because we intersect a invisible shape, we do not need add the bounces
				bounces--;

				continue;
			}

			// get the surface properties from material which the ray intersect
			const auto surface_properties =
				interaction->entity->component<material>()->build_surface_properties(interaction.value(), mode);

			// get the scattering functions from surface properties
			const auto& scattering_functions = surface_properties.functions;

			vertices.push_back(create_surface_vertex(interaction.value(), surface_properties, tracing_info.medium,
				tracing_info.beta, vertices.back(), forward_pdf));

			const auto scattering_sample = scattering_functions.sample(interaction.value(), samplers.sampler2d->next());

			if (scattering_sample.value.is_black() || scattering_sample.pdf == 0) break;
			
			tracing_info.beta *= scattering_sample.value * math::abs(dot(scattering_sample.wi, interaction->shading_space.z())) / scattering_sample.pdf;
			tracing_info.beta *= correct_shading_normal(interaction.value(), interaction->wo, scattering_sample.wi, mode);
			
			const auto wi = world_to_local(interaction->shading_space, scattering_sample.wi);
			const auto wo = world_to_local(interaction->shading_space, interaction->wo);
			
			forward_pdf = scattering_sample.pdf;
			reverse_pdf = scattering_functions.pdf(wi, wo);

			// if the bsdf we sample is specular, the pdf will be set zero and the delta is marked with this vertex
			if (has(scattering_sample.type, scattering_type::specular)) {
				vertices.back().delta = true;
				forward_pdf = 0;
				reverse_pdf = 0;
			}

			tracing_info.ray = interaction->spawn_ray(scattering_sample.wi);

			// update the medium property when interaction->entity has media
			// if interaction->normal dot ray.direction > 0, the medium we tracing should be outside of entity
			// otherwise the medium should be inside
			if (interaction->entity->has_component<cpus::media::media>())
				tracing_info.medium = medium_info(interaction->entity, interaction->normal, tracing_info.ray.direction);

			auto& last = vertices[vertices.size() - 2];

			last.reverse_pdf = vertices.back().convert_density(reverse_pdf, last);
		}
	}
	
	inline std::vector<vertex> generate_camera_sub_path(
		const std::shared_ptr<camera>& camera,
		const std::shared_ptr<scene>& scene,
		const sampler_group& samplers,
		const vector2& position,
		size_t max_depth)
	{
		// create the camera sub path, if the max_depth is 0 we will return empty sub path
		if (max_depth == 0) return {};

		// sample the camera to find the ray
		const auto ray = camera->sample(position, samplers.sampler2d->next());
		const auto beta = spectrum(1);
		
		std::vector<vertex> vertices;

		vertices.push_back(create_camera_vertex(camera, beta, ray));

		const auto [pdf_position, pdf_direction] = camera->pdf(ray);
		
		generate_sub_path(scene, samplers, transport_mode::radiance, medium_info(), beta, ray, pdf_direction, max_depth - 1, vertices);

		return vertices;
	}

	inline std::vector<vertex> generate_emitter_sub_path(
		const std::shared_ptr<scene>& scene,
		const sampler_group& samplers,
		size_t max_depth)
	{
		// create the emitter sub path, if the max_depth is 0 we will return empty sub path
		if (max_depth == 0) return {};

		// uniform sample the emitters and sample the ray from the emitter
		const auto [emitter, pdf] = uniform_sample_one_emitter(scene, samplers);

		const auto ray_sample = emitter->sample<emitters::emitter>(samplers.sampler2d->next(), samplers.sampler2d->next());

		if (ray_sample.intensity.is_black() || ray_sample.pdf_position == 0 || ray_sample.pdf_direction == 0) return {};

		const auto beta = ray_sample.intensity * math::abs(dot(ray_sample.normal, ray_sample.ray.direction)) /
			(pdf * ray_sample.pdf_position * ray_sample.pdf_direction);

		const auto medium = emitter->has_component<cpus::media::media>() ? 
			medium_info(emitter, ray_sample.normal, ray_sample.ray.direction) :
			medium_info();
		
		std::vector<vertex> vertices;

		vertices.push_back(create_emitter_vertex(emitter, ray_sample, medium));

		generate_sub_path(scene, samplers, transport_mode::important, medium, beta,
			ray_sample.ray, ray_sample.pdf_direction, max_depth - 1, vertices);

		// if the start emitter is environment emitter, the forward pdf should be he pdf of position
		// todo : add more text
		if (vertices[0].is_environment_emitter()) {
			
			if (vertices.size() > 1) {
				vertices[1].forward_pdf = ray_sample.pdf_position;

				if (vertices[1].on_surface())
					vertices[1].forward_pdf = vertices[1].forward_pdf * math::abs(dot(ray_sample.ray.direction, vertices[1].interaction().normal));
			}

			vertices[0].forward_pdf = vertices[0].pdf_emitter_environment(scene, ray_sample.ray.direction);
		}

		return vertices;
	}

	inline real remapped_value(real value)
	{
		return value != 0 ? value : 1;
	}

	inline real mis_weight(
		const std::vector<vertex>& emitter_sub_path,
		const std::vector<vertex>& camera_sub_path,
		size_t emitter_count, size_t camera_count)
	{
		if (emitter_count + camera_count == 2) return 1;

		// evaluate the mis weight of current path :
		// we reference emitter_sub_path as q_path and camera_sub_path as p_path
		// we reference emitter_count as q and camera_count as p
		// so the path is q_path[0 .. q - 1] -> p_path[p - 1 .. 0]

		// we reference x(0) .. x(n - 1) as q_path[0] .. q_path[q - 1] p_path[p - 1] .. p_path[0]
		// the pdf(q) = x(0).forward_pdf * ... * x(q - 1).forward_pdf ... * x(q).reverse_pdf * ... * x(n - 1).reverse_pdf
		// the pdf(i) = x(0).forward_pdf * ... * x(i - 1).forward_pdf ... * x(i).reverse_pdf * ... * x(n - 1).reverse_pdf
		// the pdf(q) is the pdf(s) where i = q

		// the balance heuristic weight is : pdf(q) / (pdf(0) + pdf(1) + ... + pdf(n - 1))
		// pdf(q) / (pdf(0) + pdf(1) + ... + pdf(n - 1)) = (pdf(0) / pdf(q) + ... + pdf(q - 1) / pdf(q) + 1 + pdf(q + 1) / pdf(q) + ... + pdf(n - 1) / pdf(q)) ^ -1
		// we set r(i) = pdf(i) / pdf(q), the balance heuristic weight is 1 / (r(0) + ... + r(q - 1) + r(q) + r(q + 1) ... + r(n - 1))

		// if i = q, r(i) = 1
		// if i < q, r(i) = r(i + 1) * x(i + 0).reverse_pdf / x(i + 0).forward_pdf
		// if i > q, r(i) = r(i - 1) * x(i - 1).forward_pdf / x(i - 1).reverse_pdf
		
		real sum_ri = 0;
		
		real emitter_ri = 1;

		// build the r(0) + ... + r(q - 1), loop i from q to 0
		for (auto index = static_cast<int>(emitter_count - 1); index >= 0; index--) {
			emitter_ri = emitter_ri * remapped_value(emitter_sub_path[index].reverse_pdf) / remapped_value(emitter_sub_path[index].forward_pdf);

			const auto is_last_delta = index > 0 ?
				emitter_sub_path[index - 1].delta :
				emitter_sub_path[0].is_delta_emitter();

			if (emitter_sub_path[index].delta == false && is_last_delta == false)
				sum_ri = sum_ri + emitter_ri;
		}

		real camera_ri = 1;

		// build the r(q + 1) ... r(n - 1), loop i from q + 1 to r(n - 1)
		// the p_path[0] is camera and we can not intersect it. so we ignore it
		for (auto index = static_cast<int>(camera_count - 1); index > 0; index--) {
			// p_path[i].reverse_pdf is the pdf from next(i + 1) vertex to this vertex
			// p_path[i].forward_pdf is the pdf from this vertex to next vertex(i + 1)
			// because the direction is from p_path[i + 1] - p_path[i]
			// the x(i - 1).forward_pdf is from p_path[i + 1] to p_path[i + 0]
			// the x(i - 1).reverse_pdf is from p_path[i + 0] to p_path[i + 1]
			// x(i - 1).forward_pdf = p_path[i].reverse_pdf
			// x(i - 1).reverse_pdf = p_path[i].forward_pdf
			camera_ri = camera_ri * remapped_value(camera_sub_path[index].reverse_pdf) / remapped_value(camera_sub_path[index].forward_pdf);

			if (camera_sub_path[index].delta == false && camera_sub_path[index - 1].delta == false)
				sum_ri = sum_ri + camera_ri;
		}

		return 1 / (1 + sum_ri);
	}

	inline real mis_weight_full_camera_path_case(
		const std::shared_ptr<scene>& scene,
		std::vector<vertex>& emitter_sub_path,
		std::vector<vertex>& camera_sub_path,
		size_t emitter_count, size_t camera_count)
	{
		// emitter_count == 0 and camera_count >= 2
		
		if (emitter_count + camera_count == 2) return 1;

		auto& this_camera = camera_sub_path[camera_count - 1];
		auto& last_camera = camera_sub_path[camera_count - 2];

		const auto assignment0 = scope_assignment_t<bool>(&this_camera.delta, false);

		const auto assignment1 = scope_assignment_t<real>(&this_camera.reverse_pdf, this_camera.pdf_emitter_origin(scene, last_camera));
		const auto assignment2 = scope_assignment_t<real>(&last_camera.reverse_pdf, this_camera.pdf(scene, last_camera));

		return mis_weight(emitter_sub_path, camera_sub_path, emitter_count, camera_count);
	}

	inline real mis_weight_emitter_case(
		const std::shared_ptr<scene>& scene, const vertex& sampled_vertex,
		std::vector<vertex>& emitter_sub_path,
		std::vector<vertex>& camera_sub_path,
		size_t emitter_count, size_t camera_count)
	{
		// emitter_count = 1 and camera_count > 1
		// because emitter_count = 1 and camera_count = 1 is invalid
		
		if (emitter_count + camera_count == 2) return 1;

		auto& this_emitter = emitter_sub_path[emitter_count - 1];
		
		auto& this_camera = camera_sub_path[camera_count - 1];
		auto& last_camera = camera_sub_path[camera_count - 2];

		const auto assignment0 = scope_assignment_t<vertex>(&this_emitter, sampled_vertex);

		const auto assignment1 = scope_assignment_t<bool>(&this_emitter.delta, false);
		const auto assignment2 = scope_assignment_t<bool>(&this_camera.delta, false);

		const auto assignment3 = scope_assignment_t<real>(&this_emitter.reverse_pdf, this_camera.pdf(last_camera, this_emitter));
		const auto assignment4 = scope_assignment_t<real>(&this_camera.reverse_pdf, this_emitter.pdf(scene, this_camera));
		const auto assignment5 = scope_assignment_t<real>(&last_camera.reverse_pdf, this_camera.pdf(this_emitter, last_camera));
		
		return mis_weight(emitter_sub_path, camera_sub_path, emitter_count, camera_count);
	}

	inline real mis_weight_camera_case(
		const std::shared_ptr<scene>& scene, const vertex& sampled_vertex,
		std::vector<vertex>& emitter_sub_path,
		std::vector<vertex>& camera_sub_path,
		size_t emitter_count, size_t camera_count)
	{
		// emitter_count > 1 and camera_count = 1
		// because emitter_count = 1 and camera_count = 1 is invalid

		if (emitter_count + camera_count == 2) return 1;

		auto& this_emitter = emitter_sub_path[emitter_count - 1];
		auto& last_emitter = emitter_sub_path[emitter_count - 2];

		auto& this_camera = camera_sub_path[camera_count - 1];

		const auto assignment0 = scope_assignment_t<vertex>(&this_camera, sampled_vertex);

		const auto assignment1 = scope_assignment_t<bool>(&this_emitter.delta, false);
		const auto assignment2 = scope_assignment_t<bool>(&this_camera.delta, false);

		const auto assignment3 = scope_assignment_t<real>(&this_emitter.reverse_pdf, this_camera.pdf(this_emitter));
		const auto assignment4 = scope_assignment_t<real>(&last_emitter.reverse_pdf, this_emitter.pdf(this_camera, last_emitter));
		const auto assignment5 = scope_assignment_t<real>(&this_camera.reverse_pdf, this_emitter.pdf(last_emitter, this_camera));

		return mis_weight(emitter_sub_path, camera_sub_path, emitter_count, camera_count);
	}

	inline real mis_weight_common_case(
		const std::shared_ptr<scene>& scene,
		std::vector<vertex>& emitter_sub_path,
		std::vector<vertex>& camera_sub_path,
		size_t emitter_count, size_t camera_count)
	{
		// emitter_count > 1 and camera_count > 1

		auto& this_emitter = emitter_sub_path[emitter_count - 1];
		auto& last_emitter = emitter_sub_path[emitter_count - 2];

		auto& this_camera = camera_sub_path[camera_count - 1];
		auto& last_camera = camera_sub_path[camera_count - 2];
		
		const auto assignment0 = scope_assignment_t<bool>(&this_emitter.delta, false);
		const auto assignment1 = scope_assignment_t<bool>(&this_camera.delta, false);

		const auto assignment2 = scope_assignment_t<real>(&this_emitter.reverse_pdf, this_camera.pdf(last_camera, this_emitter));
		const auto assignment3 = scope_assignment_t<real>(&last_emitter.reverse_pdf, this_emitter.pdf(this_camera, last_emitter));
		const auto assignment4 = scope_assignment_t<real>(&this_camera.reverse_pdf, this_emitter.pdf(last_emitter, this_camera));
		const auto assignment5 = scope_assignment_t<real>(&last_camera.reverse_pdf, this_camera.pdf(this_emitter, last_camera));

		return mis_weight(emitter_sub_path, camera_sub_path, emitter_count, camera_count);
	}
	
	inline spectrum connect_sub_path(
		const std::shared_ptr<camera>& camera,
		const std::shared_ptr<scene>& scene,
		const sampler_group& samplers,
		std::vector<vertex>& emitter_sub_path,
		std::vector<vertex>& camera_sub_path,
		size_t emitter_count,  size_t camera_count, vector2& position)
	{
		// if camera_sub_path[camera_count - 1].type is emitter, means it is the last vertex in camera sub path
		// because the type of vertex between begin and end must be surface or medium
		// so if the emitter_count is not 0, the connect will be invalid
		// but if the last vertex is not emitter vertex(in this case, the tracing ending because of max_depth), the connect will be valid.
		if (camera_count > 1 && emitter_count != 0 && camera_sub_path[camera_count - 1].type == vertex_type::emitter)
			return spectrum(0);

		auto L = spectrum(0);

		// when the emitter count is 0, means we will use the camera sub path as the full path
		// so we evaluate this path(emitter_count + camera_count >= 2)
		if (emitter_count == 0) {
			const auto& this_camera = camera_sub_path[camera_count - 1];
			const auto& last_camera = camera_sub_path[camera_count - 2];

			// because the camera sub path is the full path, so we will use the end vertex of sub path as emitter
			// if the vertex has emitter, we will evaluate the intensity reference last vertex(from emitter to last vertex)
			// and current.beta is the value of beta from start vertex to this vertex
			// so the L should be current.beta * the intensity from current vertex(the direction should from current to last)
			L = this_camera.evaluate(scene, last_camera) * this_camera.beta;

			if (L.is_black()) return L;
			
			const auto weight = mis_weight_full_camera_path_case(scene,
				emitter_sub_path, camera_sub_path, emitter_count, camera_count);
			
			return L * weight;
		}

		// when the emitter_count is 1, means we will connect a emitter vertex with camera sub path
		// so we need sample the emitter and connect it
		if (emitter_count == 1) {
			const auto current_vertex = camera_sub_path[camera_count - 1];

			// if the current_emitter can not be connected, we will return 0
			if (!current_vertex.connectible()) return spectrum(0);

			// sample the emitter
			const auto [emitter, pdf] = uniform_sample_one_emitter(scene, samplers);
			const auto emitter_sample = emitter->sample<emitters::emitter>(current_vertex.interaction(), samplers.sampler2d->next());

			if (!emitter_sample.intensity.is_black() && emitter_sample.pdf > 0) {
				auto sampled_vertex = create_emitter_vertex(emitter, emitter_sample.interaction,
					emitter_sample.intensity / (pdf * emitter_sample.pdf), 0);

				// todo : add some text
				sampled_vertex.forward_pdf = sampled_vertex.pdf_emitter_origin(scene, current_vertex);

				// current_vertex.beta is the beta value from camera to current_vertex
				// the sampled_vertex.beta is the beta value(intensity or radiance / pdf)
				// so the L should be current_vertex.beta * sampled_vertex.beta * beta from current_vertex to sampled_vertex
				// so we will use current_vertex.evaluate() to evaluate the beta
				// because the path is from camera to emitter, so we will use transport_mode::radiance
				L = current_vertex.beta * current_vertex.evaluate(sampled_vertex, transport_mode::radiance) * sampled_vertex.beta;

				// if current is on the surface, we need consider dot value between the normal and wi
				if (current_vertex.on_surface()) L *= math::abs(dot(emitter_sample.wi, current_vertex.shading_normal()));

				if (L.is_black()) return L;

				// visible test
				/*const auto shadow_ray = current_vertex.interaction().spawn_ray_to(sampled_vertex.interaction().point);
				const auto shadow_interaction = scene->intersect_with_shadow_ray(shadow_ray);

				if (shadow_interaction.has_value()) return spectrum(0);*/
				const auto beam = current_vertex.evaluate_media_beam(scene, samplers, sampled_vertex.interaction());
				
				const auto weight = mis_weight_emitter_case(scene, sampled_vertex, 
					emitter_sub_path, camera_sub_path, emitter_count, camera_count);

				return L * weight * beam;
			}
		}

		// when the camera_count is 1, means we will connect the camera vertex with emitter sub path
		// so we need sample the camera and connect it
		if (camera_count == 1) {
			const auto current_vertex = emitter_sub_path[emitter_count - 1];

			// if the current_emitter can not be connected, we will return 0
			if (!current_vertex.connectible()) return spectrum(0);

			const auto camera_sample = camera->sample(current_vertex.interaction(), samplers.sampler2d->next());

			// because we sample the camera, so the position of the ray start on film is changed
			// we need update the position
			position = camera_sample.point;

			if (!camera_sample.value.is_black() && camera_sample.pdf > 0) {
				// create a camera vertex and connect it with emitter sub path
				const auto sampled_vertex = create_camera_vertex(camera, camera_sample.interaction, camera_sample.value / camera_sample.pdf);

				// the current_vertex.beta is the value from start of path to current vertex
				// and sampled_vertex.beta is the value of this vertex
				// so the L should be the current_vertex.beta * sampled_vertex.beta * beta from current_vertex to sampled_vertex
				// so we will use current_vertex.evaluate() to evaluate the beta
				// because the path is from emitter to camera, so we will use transport_mode::important
				L = current_vertex.beta * current_vertex.evaluate(sampled_vertex, transport_mode::important) * sampled_vertex.beta;

				// if current is on the surface, we need consider dot value between the normal and wi
				if (current_vertex.on_surface()) L *= math::abs(dot(camera_sample.wi, current_vertex.shading_normal()));

				if (L.is_black()) return L;

				// visible test
				/*const auto shadow_ray = current_vertex.interaction().spawn_ray_to(sampled_vertex.interaction().point);
				const auto shadow_interaction = scene->intersect_with_shadow_ray(shadow_ray);

				if (shadow_interaction.has_value()) return spectrum(0);*/

				const auto beam = current_vertex.evaluate_media_beam(scene, samplers, sampled_vertex.interaction());
				
				const auto weight = mis_weight_camera_case(scene, sampled_vertex,
					emitter_sub_path, camera_sub_path, emitter_count, camera_count);
				
				return L * weight * beam;
			}

			return spectrum(0);
		}

		// now, connect the two sub path
		const auto this_emitter = emitter_sub_path[emitter_count - 1];
		const auto this_camera = camera_sub_path[camera_count - 1];

		// if the current_emitter or current_camera can not be connected, we will return 0
		if (!this_camera.connectible() || !this_emitter.connectible()) 
			return spectrum(0);

		// current_emitter.beta is the value from emitter to current_emitter
		// current_camera.beta is the value from camera to current_camera
		// so we need the beta of two vertex(current_emitter and current_camera)
		// we reference current_emitter as q and current_camera as p
		// and q - 1 is the prev vertex in emitter sub path
		// and p - 1 is the prev vertex in camera sub path
		// the beta of two vertex is F(q - 1, q, p) * F(p - 1, p, q)
		// so current_emitter.evaluate() is F(q - 1, q, p) and current_camera.evaluate() is F(p - 1, p, q)
		L = this_emitter.beta * this_emitter.evaluate(this_camera, transport_mode::important) *
			this_camera.evaluate(this_emitter, transport_mode::radiance) * this_camera.beta;

		if (L.is_black()) return L;

		/*const auto shadow_ray = current_emitter.interaction().spawn_ray_to(current_camera.interaction().point);
		const auto shadow_interaction = scene->intersect_with_shadow_ray(shadow_ray);

		if (shadow_interaction.has_value()) return spectrum(0);*/

		const auto beam = this_emitter.evaluate_media_beam(scene, samplers, this_camera.interaction());
		
		// G = V * T * C(p0, p1) * C(p1, p0) / distance_squared(p0 - p1)
		// C(p0, p1) = abs(normal of p0 dot normalize(p0 - p1)) if p0 is on surface otherwise C = 1
		// T = transmittance between p0 and p1
		// V = 1 if the ray is not occluded otherwise V = 0 
		const auto w = normalize(this_emitter.interaction().point - this_camera.interaction().point);
		const auto inv_distance_2 = 1 / distance_squared(this_emitter.interaction().point, this_camera.interaction().point);

		if (this_emitter.on_surface()) L *= math::abs(dot(w, this_emitter.shading_normal()));
		if (this_camera.on_surface()) L *= math::abs(dot(w, this_camera.shading_normal()));
		
		L *= inv_distance_2;

		// the pdf will be computed and used in MIS weight
		const auto weight = mis_weight_common_case(scene,
			emitter_sub_path, camera_sub_path, emitter_count, camera_count);
		
		return L * weight * beam;
	}

}

rainbow::cpus::integrators::bidirectional_path_integrator::bidirectional_path_integrator(
	const std::shared_ptr<sampler2d>& sampler2d, 
	const std::shared_ptr<sampler1d>& sampler1d,
	size_t max_depth) :
	mSampler2D(sampler2d), mSampler1D(sampler1d), mMaxDepth(max_depth)
{
}

void rainbow::cpus::integrators::bidirectional_path_integrator::render(
	const std::shared_ptr<camera>& camera,
	const std::shared_ptr<scene>& scene)
{
	const auto film = camera->film();
	const auto bound = film->pixels_bound();

	const auto bound_size = vector2i(
		bound.max.x - bound.min.x,
		bound.max.y - bound.min.y);

	const auto tile_size = static_cast<size_t>(16);
	const auto tile_count = vector2i(
		(bound_size.x + tile_size - 1) / tile_size,
		(bound_size.y + tile_size - 1) / tile_size);

	const auto sample_count =
		static_cast<size_t>(bound_size.x) *
		static_cast<size_t>(bound_size.y) *
		mSampler2D->samples_per_pixel();

	struct parallel_input {
		size_t tile_index;

		bound2i tile;
	};

	struct parallel_output {
		film_tile tile;
	};

	auto outputs = std::vector<parallel_output>();
	auto inputs = std::vector<parallel_input>();

	for (size_t y = bound.min.y; y < bound.max.y; y += tile_size) {
		for (size_t x = bound.min.x; x < bound.max.x; x += tile_size) {
			const auto min_range = vector2i(x, y);
			const auto max_range = vector2i(
				min(static_cast<int>(x + tile_size), bound.max.x),
				min(static_cast<int>(y + tile_size), bound.max.y)
			);

			const auto sample_bound = bound2i(min_range, max_range);

			inputs.push_back({ inputs.size(), sample_bound });
			outputs.push_back({ film_tile(sample_bound, film) });
		}
	}

#ifdef _DEBUG
	std::set<std::pair<int, int>> debug_pixel_lists;

	for (const auto& debug_pixel : mDebugPixels)
		debug_pixel_lists.insert({ debug_pixel.x, debug_pixel.y });
#endif
	
#ifdef __PARALLEL_RENDER__
	const auto execution_policy = std::execution::par;
#else
	const auto execution_policy = std::execution::seq;
#endif

	const auto samples_per_pixel = mSampler2D->samples_per_pixel();

	logs::info("start rendering...");
	logs::info("image min range : x = {0}, y = {1}.", bound.min.x, bound.min.y);
	logs::info("image max range : x = {0}, y = {1}.", bound.max.x, bound.max.y);
	logs::info("tile size : width = {0}, height = {1}.", tile_size, tile_size);

	std::atomic_int finished_tile_count = 0;

	const auto start_rendering_time = std::chrono::high_resolution_clock::now();

	std::for_each(execution_policy, inputs.begin(), inputs.end(), [&](const parallel_input& input)
		{
			const auto seed = input.tile_index;
			const auto generator = std::make_shared<random_generator>(seed);

			const auto trace_samplers = sampler_group(
				mSampler1D->clone(generator),
				mSampler2D->clone(generator));
				
			for (auto y = input.tile.min.y; y < input.tile.max.y; y++) {
				for (auto x = input.tile.min.x; x < input.tile.max.x; x++) {
					trace_samplers.reset();

					for (size_t index = 0; index < samples_per_pixel; index++) {
						const auto position = vector2i(x, y);
						const auto sample = vector2(x, y) + trace_samplers.sampler2d->next();

						const auto debug = integrator_debug_info(position, index);

#ifdef _DEBUG
						// when mDebugPixels is not empty, the debug_pixel_lists is not empty too.
						// when debug_pixel_lists is not empty, we will only trace the pixel that in the debug lists
						// in other words, the pixels we called integrator::set_debug_trace_pixel().
						if (!debug_pixel_lists.empty() && debug_pixel_lists.find({ x, y }) == debug_pixel_lists.end()) {

							// we do not trace these sample, but the filter weight can not be zero
							// so we will set the sample value to zero.
							outputs[input.tile_index].tile.add_sample(sample, 0);

							trace_samplers.next_sample();

							break;
						}
#endif					

						auto emitter_sub_path = generate_emitter_sub_path(scene, trace_samplers, mMaxDepth + 1);
						auto camera_sub_path = generate_camera_sub_path(camera, scene, trace_samplers, sample, mMaxDepth + 2);

						auto L = spectrum(0);

						// loop the camera sub path and emitter sub path and try to connect them
						for (auto camera_count = 1; camera_count <= camera_sub_path.size(); camera_count++) {
							for (auto emitter_count = 0; emitter_count <= emitter_sub_path.size(); emitter_count++) {
								
								const auto depth = camera_count + emitter_count - 2;

								// if the depth of path less than 2 or grater than max_depth the connected path is invalid
								// so we will skip this path
								if (depth < 0 || depth > mMaxDepth || (camera_count == 1 && emitter_count == 1))
									continue;

								// record the sample position, if the camera_count is 1 the path will sample the camera
								// the sample position will change
								// (we need to know the new sample position and add the spectrum to the position of sample_position)
								auto sample_position = sample;

								// try to connect the camera sub path and emitter sub path
								const auto value = connect_sub_path(camera, scene, trace_samplers,
									emitter_sub_path, camera_sub_path,
									emitter_count, camera_count, sample_position);

								// if camera_count is 1, the value should be add to film directly
								// and we will use the samples per pixel to weight the value
								// otherwise, add this value into L
								if (camera_count == 1) {
									const auto inv_weight = static_cast<real>(1) / samples_per_pixel;

									film->add_pixel(floor(sample_position), value * inv_weight);
								}
								else L += value;
							}
						}

						outputs[input.tile_index].tile.add_sample(sample, L);
						
						trace_samplers.next_sample();
					}
				}
			}

			logs::info("finish tile {0}, finished {1} / total : {2}", input.tile_index, ++finished_tile_count, inputs.size());
		});

	for (size_t index = 0; index < outputs.size(); index++)
		film->add_tile(outputs[index].tile);

	const auto end_rendering_time = std::chrono::high_resolution_clock::now();

	logs::info("finish rendering..., time used {0}s.",
		std::chrono::duration_cast<std::chrono::duration<double>>(end_rendering_time - start_rendering_time).count());
}
