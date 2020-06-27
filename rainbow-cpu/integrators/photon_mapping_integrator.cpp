#include "photon_mapping_integrator.hpp"

#include <unordered_map>
#include <execution>
#include <atomic>

namespace rainbow::cpus::integrators {

	struct visible_point {
		scattering_function_collection functions;
		
		spectrum beta = spectrum(0);
		
		vector3 point = vector3(0);
		vector3 wo = vector3(0);

		visible_point() = default;

		visible_point(
			const scattering_function_collection& functions,
			const spectrum& beta, const vector3& point, const vector3& wo) :
			functions(functions), beta(beta), point(point), wo(wo)
		{
		}
	};

	struct mapping_pixel {
		std::optional<visible_point> point = std::nullopt;

		spectrum tau = spectrum(0);
		spectrum L = spectrum(0);

		real radius = 0;
		real n = 0;


		std::atomic<spectrum> phi;
		std::atomic<size_t> m;
		
		mapping_pixel() = default;
	};

	struct visible_point_grid {
		std::unordered_map<size_t, std::vector<mapping_pixel*>> pool;
		
		vector3_t<size_t> size = vector3_t<size_t>(0);

		bound3 bound;

		visible_point_grid() = default;
	};

	inline bool position_in_grid(const visible_point_grid& grid, const vector3& position)
	{
		if (position.x < grid.bound.min.x || position.x > grid.bound.max.x) return false;
		if (position.y < grid.bound.min.y || position.y > grid.bound.max.y) return false;
		if (position.z < grid.bound.min.z || position.z > grid.bound.max.z) return false;

		return true;
	}
	
	inline vector3_t<size_t> position_to_grid(const visible_point_grid& grid, const vector3& position)
	{
		const auto position_offset_grid = position - grid.bound.min;

		return vector3_t<size_t>(
			position_offset_grid.x / (grid.bound.max.x - grid.bound.min.x) * grid.size.x,
			position_offset_grid.y / (grid.bound.max.y - grid.bound.min.y) * grid.size.y,
			position_offset_grid.z / (grid.bound.max.z - grid.bound.min.z) * grid.size.z
		);
	}

	inline size_t grid_to_index(const visible_point_grid& grid, const vector3_t<size_t>& position)
	{
		return position.z * grid.size.x * grid.size.y + position.y * grid.size.x + position.x;
	}
	
	inline std::tuple<std::optional<visible_point>, spectrum> trace_visible_point(
		const std::shared_ptr<scene>& scene,
		const integrator_debug_info& debug,
		const sampler_group& samplers,
		const ray& first_ray, size_t max_depth)
	{
		path_tracing_info tracing_info;

		tracing_info.specular = false;
		tracing_info.ray = first_ray;
		tracing_info.value = 0;
		tracing_info.beta = 1;

		std::optional<visible_point> point = std::nullopt;
		
		for (auto bounces = static_cast<int>(0); bounces < max_depth; bounces++) {
			const auto interaction = scene->intersect(tracing_info.ray);

			// if we do not find the shape that the ray intersect we can end this tracing
			if (!interaction.has_value()) {
				
				for (const auto& light : scene->environments())
					tracing_info.value += tracing_info.beta * light->component<emitter>()->evaluate(
						shared::interaction(), -tracing_info.ray.direction);

				break;
			}

			// when the first ray intersect a light, we will evaluate the intensity of it
			// when the specular ray intersect an emitter, we will evaluate the intensity of it
			// because the specular ray we do not solve it at "uniform_sample_one_emitter".
			if ((bounces == 0 || tracing_info.specular) && interaction->entity->has_component<emitter>())
				tracing_info.value += tracing_info.beta * interaction->entity->evaluate<emitter>(interaction.value(), -tracing_info.ray.direction);

			// when the material is nullptr, we can think it is a invisible entity
			// we will continue spawn a ray without changing the direction
			if (!interaction->entity->has_component<material>()) {
				// compute the new ray 
				tracing_info.ray = interaction->spawn_ray(tracing_info.ray.direction);

				// because we intersect a invisible shape, we do not need add the bounces
				bounces--;

				continue;
			}

			// get the surface properties from material which the ray intersect
			// if the entity does not have material, we return default surface properties(0 functions)
			const auto surface_properties =
				interaction->entity->component<material>()->build_surface_properties(interaction.value());

			// get the scattering functions from surface properties
			const auto& scattering_functions = surface_properties.functions;

			// we will sample the emitters to compute the path contribution
			// when the functions do not have any functions without specular we do not sample it
			// because the f(wo, wi) of specular functions is 0, the result must be 0.
			if (scattering_functions.count(scattering_type::all ^ scattering_type::specular) != 0)
				tracing_info.value += tracing_info.beta * uniform_sample_one_emitter(
					scene, samplers, tracing_info, interaction.value(), scattering_functions, false);

			const auto is_diffuse = scattering_functions.count(
				scattering_type::diffuse | scattering_type::reflection | scattering_type::transmission) > 0;

			const auto is_glossy = scattering_functions.count(
				scattering_type::glossy | scattering_type::reflection | scattering_type::transmission) > 0;

			// if the surface is diffuse or glossy(it is the last bounce), we will break this loop
			// and use this vertex as visible point
			if (is_diffuse || (is_glossy && bounces == max_depth - 1)) {
				point = visible_point(
					surface_properties.functions, 
					tracing_info.beta, 
					interaction->point, 
					-tracing_info.ray.direction
				);
				
				break;
			}

			const auto scattering_sample = scattering_functions.sample(interaction.value(), samplers.sampler2d->next());

			if (scattering_sample.value.is_black() || scattering_sample.pdf == 0) break;

			tracing_info.beta *= scattering_sample.value * math::abs(dot(scattering_sample.wi, interaction->shading_space.z())) / scattering_sample.pdf;

			// indicate the path it is specular or not.
			// if it is, we will evaluate it when we intersect an emitter at next bounce
			tracing_info.specular = has(scattering_sample.type, scattering_type::specular);

			const auto luminance = tracing_info.beta.luminance();
			
			if (luminance < static_cast<real>(0.25)) {
				const auto prob = min(static_cast<real>(1), luminance);

				if (samplers.sampler1d->next().x > prob) break;

				tracing_info.beta /= prob;
			}

			tracing_info.ray = interaction->spawn_ray(scattering_sample.wi);
		}

		return { point, tracing_info.value };
	}

	inline visible_point_grid build_visible_point_grid(std::vector<mapping_pixel>& pixels)
	{
		visible_point_grid grid;

		real max_radius = 0;

		grid.bound.min = vector3(std::numeric_limits<real>::max());
		grid.bound.max = vector3(std::numeric_limits<real>::min());

		// compute the bound of grid and the max_radius of visible points
		for (const auto& pixel : pixels) {
			if (!pixel.point.has_value()) continue;

			const auto pixel_bound = bound3(
				pixel.point->point - pixel.radius, 
				pixel.point->point + pixel.radius);

			grid.bound.union_it(pixel_bound);

			max_radius = max(max_radius, pixel.radius);
		}

		const auto diagonal = grid.bound.max - grid.bound.min;
		const auto max_diagonal = max_component(diagonal);

		// the size of grid cell is max_radius in the axis max_diagonal on
		// and we also try to keep the size of other axes to max_radius roughly.
		// the number of grid cells one pixel can overlap will smaller than four
		const auto base_size = static_cast<int>(max_diagonal / max_radius);

		grid.size = vector3_t<size_t>(
			max(static_cast<size_t>(base_size * diagonal.x / max_diagonal), static_cast<size_t>(1)),
			max(static_cast<size_t>(base_size * diagonal.y / max_diagonal), static_cast<size_t>(1)),
			max(static_cast<size_t>(base_size * diagonal.z / max_diagonal), static_cast<size_t>(1)));

		// loop the pixels and find voxels that in the box of visible points
		std::for_each(std::execution::seq, pixels.begin(), pixels.end(), [&](mapping_pixel& pixel)
			{
				if (!pixel.point.has_value()) return;

				// find the bound of visible points sphere, the photon that in this box will influence this visible point
				const auto min = position_to_grid(grid, pixel.point->point - vector3(pixel.radius));
				const auto max = position_to_grid(grid, pixel.point->point + vector3(pixel.radius));

				// loop the grid cell and record the pixels
				for (auto z = min.z; z <= max.z; z++) {
					for (auto y = min.y; y <= max.y; y++) {
						for (auto x = min.x; x <= max.x; x++) {
							grid.pool[grid_to_index(grid, vector3i(x, y, z))].push_back(&pixel);
						}
					}
				}
			});

		return grid;
	}

	inline void add_photon(
		const path_tracing_info& tracing_info,
		const visible_point_grid& grid,
		const interaction& interaction)
	{
		if (!position_in_grid(grid, interaction.point)) return;

		const auto position = position_to_grid(grid, interaction.point);
		const auto index = grid_to_index(grid, position);

		for (auto& pixel : grid.pool.at(index)) {
			if (distance_squared(pixel->point->point, interaction.point) > pixel->radius * pixel->radius)
				continue;

			auto value = pixel->phi.load();
			
			const auto phi = spectrum(value + tracing_info.beta *
				pixel->point->functions.evaluate(pixel->point->wo, -tracing_info.ray.direction));

			while (!pixel->phi.compare_exchange_weak(value, phi));
			
			++pixel->m;
		}
	}
	
	inline void trace_photon(
		const std::shared_ptr<scene>& scene,
		const integrator_debug_info& debug,
		const sampler_group& samplers, 
		const visible_point_grid& grid, size_t max_depth)
	{
		const auto [emitter, pdf] = uniform_sample_one_emitter(scene, samplers);
		const auto ray_sample = emitter->sample<emitters::emitter>(samplers.sampler2d->next(), samplers.sampler2d->next());

		if (ray_sample.intensity.is_black() || ray_sample.pdf_direction == 0 || ray_sample.pdf_position == 0) 
			return;

		path_tracing_info tracing_info;

		tracing_info.beta = ray_sample.intensity * math::abs(dot(ray_sample.normal, ray_sample.ray.direction)) /
			(pdf * ray_sample.pdf_direction * ray_sample.pdf_position);
		tracing_info.ray = ray_sample.ray;

		if (tracing_info.beta.is_black()) return;
		
		for (int bounces = 0; bounces < max_depth; bounces++) {
			const auto interaction = scene->intersect(tracing_info.ray);

			if (!interaction.has_value()) break;

			add_photon(tracing_info, grid, interaction.value());

			// when the material is nullptr, we can think it is a invisible entity
			// we will continue spawn a ray without changing the direction
			if (!interaction->entity->has_component<material>()) {
				// compute the new ray 
				tracing_info.ray = interaction->spawn_ray(tracing_info.ray.direction);

				// because we intersect a invisible shape, we do not need add the bounces
				bounces--;

				continue;
			}


			// get the surface properties from material which the ray intersect
			// if the entity does not have material, we return default surface properties(0 functions)
			const auto surface_properties =
				interaction->entity->component<material>()->build_surface_properties(interaction.value());

			// get the scattering functions from surface properties
			const auto& scattering_functions = surface_properties.functions;

			const auto scattering_sample = scattering_functions.sample(interaction.value(), samplers.sampler2d->next());

			if (scattering_sample.value.is_black() || scattering_sample.pdf == 0) break;

			const auto new_beta = spectrum(tracing_info.beta *
				scattering_sample.value * math::abs(dot(scattering_sample.wi, interaction->shading_space.z())) / scattering_sample.pdf);

			const auto q = max(1 - new_beta.luminance() / tracing_info.beta.luminance(), static_cast<real>(0));

			if (samplers.sampler1d->next().x < q) break;

			tracing_info.beta = new_beta / (1 - q);
			tracing_info.ray = interaction->spawn_ray(scattering_sample.wi);
		}
	}
}

rainbow::cpus::integrators::photon_mapping_integrator::photon_mapping_integrator(
	const std::shared_ptr<sampler2d>& sampler2d, 
	const std::shared_ptr<sampler1d>& sampler1d, 
	size_t iterations, size_t max_depth, size_t photons, real radius) :
	mSampler2D(sampler2d), mSampler1D(sampler1d), mIterations(iterations),
	mMaxDepth(max_depth), mPhotons(photons), mRadius(radius)
{
}

void rainbow::cpus::integrators::photon_mapping_integrator::render(
	const std::shared_ptr<camera>& camera,
	const std::shared_ptr<scene>& scene)
{
	const auto pixel_bound = camera->film()->pixels_bound();
	const auto bound_size = vector2i(pixel_bound.max.x - pixel_bound.min.x, pixel_bound.max.y - pixel_bound.min.y);

	auto pixels = std::vector<mapping_pixel>(bound_size.x * bound_size.y);
	
	for (auto& pixel : pixels) pixel.radius = mRadius;

	const auto tile_size = static_cast<size_t>(16);
	const auto tile_count = vector2i(
		(bound_size.x + tile_size - 1) / tile_size,
		(bound_size.y + tile_size - 1) / tile_size);

	struct pixel_input {
		sampler_group samplers;
		
		size_t tile_index;

		bound2i tile;
	};
	
	auto pixel_inputs = std::vector<pixel_input>();

	for (size_t y = pixel_bound.min.y; y < pixel_bound.max.y; y += tile_size) {
		for (size_t x = pixel_bound.min.x; x < pixel_bound.max.x; x += tile_size) {
			const auto min_range = vector2i(x, y);
			const auto max_range = vector2i(
				min(static_cast<int>(x + tile_size), pixel_bound.max.x),
				min(static_cast<int>(y + tile_size), pixel_bound.max.y)
			);

			const auto sample_bound = bound2i(min_range, max_range);

			const auto generator = std::make_shared<random_generator>(pixel_inputs.size());
			const auto samplers = sampler_group(mSampler1D->clone(generator), mSampler2D->clone(generator));

			pixel_inputs.push_back({ samplers, pixel_inputs.size(), sample_bound });
		}
	}

	struct photon_input {
		sampler_group samplers;

		size_t begin, end;
	};

	const auto photons = mPhotons == 0 ? pixels.size() : mPhotons;
	const auto chunk_size = static_cast<size_t>(8192);
	
	std::vector<photon_input> photon_inputs;

	for (size_t index = 0; index < photons; index += chunk_size) {
		const auto generator = std::make_shared<random_generator>(photon_inputs.size());
		const auto samplers = sampler_group(mSampler1D->clone(generator), mSampler2D->clone(generator));

		const auto begin = max(index * chunk_size, static_cast<size_t>(0));
		const auto end = min((index + 1) * chunk_size, photons);
		
		photon_inputs.push_back({ samplers, begin, end });
	}
	
	const auto execution_policy = std::execution::par;

	for (size_t iteration = 0; iteration < mIterations; iteration++) {
		// first pass, loop pixels to build the mapping_pixel and visible points
		std::for_each(execution_policy, pixel_inputs.begin(), pixel_inputs.end(), [&](const pixel_input& input)
			{
				const auto trace_samplers = input.samplers;
			
				for (auto y = input.tile.min.y; y < input.tile.max.y; y++) {
					for (auto x = input.tile.min.x; x < input.tile.max.x; x++) {

						trace_samplers.reset();

						const auto position = vector2i(x, y);
						const auto sample = vector2(x, y) + trace_samplers.sampler2d->next();

						const auto debug = integrator_debug_info(position, 0);

						const auto [point, value] = trace_visible_point(scene, debug, trace_samplers,
							camera->sample(sample, trace_samplers.sampler2d->next()), mMaxDepth);

						const auto offset = y * bound_size.x + x;

						pixels[offset].point = point;
						pixels[offset].L += value;
					}
				}
			});

		// second pass, grid visible pixels
		const auto grid = build_visible_point_grid(pixels);

		// third pass, tracing the photon
		std::for_each(execution_policy, photon_inputs.begin(), photon_inputs.end(), [&](const photon_input& input)
			{
				for (auto index = input.begin; index < input.end; index++) {
					trace_photon(scene, integrator_debug_info(vector2i(), index),
						input.samplers, grid, mMaxDepth);
				}
			});

		const auto gamma = static_cast<real>(2) / 3;
		
		std::for_each(execution_policy, pixels.begin(), pixels.end(), [&](mapping_pixel& pixel)
			{
				if (pixel.m > 0) {
					const auto new_n = pixel.n + gamma * pixel.m;
					const auto new_r = pixel.radius * math::sqrt(new_n / (pixel.n + pixel.m));
					const auto phi = pixel.phi.load();

					pixel.tau = (pixel.tau + pixel.point->beta * phi) * (new_r * new_r) / (pixel.radius * pixel.radius);

					pixel.radius = new_r;
					pixel.n = new_n;
					pixel.m = 0;

					pixel.phi = spectrum(0);
				}

				pixel.point = std::nullopt;
			});
	}

	const auto film = camera->film();

	for (size_t index = 0; index < pixels.size(); index++) {
		const auto& pixel = pixels[index];
		const auto value = pixel.L / static_cast<real>(mIterations) + 
			pixel.tau / (mIterations * photons * pi<real>() * pixel.radius * pixel.radius);

		film->set_pixel(index, value);
	}
}
