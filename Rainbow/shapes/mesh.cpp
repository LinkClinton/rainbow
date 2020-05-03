#include "mesh.hpp"

#include "../scatterings/scattering_function.hpp"
#include "../shared/sample_function.hpp"
#include "../shared/logs/log.hpp"

#ifdef __GLTF_LOADER__

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_JSON

#include <nlohmann/json.hpp>
#include <tiny_gltf.h>

#endif

rainbow::shapes::mesh::mesh(
	const std::vector<vector3>& positions,
	const std::vector<vector3>& tangents,
	const std::vector<vector3>& normals,
	const std::vector<vector3>& uvs,
	const std::vector<unsigned>& indices,
	bool reverse_orientation) : shape(reverse_orientation),
	mPositions(positions), mTangents(tangents), mNormals(normals), mUVs(uvs),
	mIndices(indices), mFaceCount(mIndices.size() / 3), mArea(0)
{
	for (size_t index = 0; index < mFaceCount; index++)
		mArea = mArea + face_area(index);
}

std::optional<rainbow::surface_interaction> rainbow::shapes::mesh::intersect(const ray& ray) const
{
	std::optional<surface_interaction> nearest_interaction;

	for (size_t index = 0; index < mFaceCount; index++) {
		const auto interaction = intersect_with_triangle(ray, index);

		if (interaction.has_value()) nearest_interaction = interaction;
	}
	
	return nearest_interaction;
}

rainbow::shapes::shape_sample rainbow::shapes::mesh::sample(const vector2& sample) const
{
	const auto which = std::min(static_cast<size_t>(std::floor(sample.x * mFaceCount)),
		mFaceCount - 1);

	const auto sample_remapped = vector2(
		std::min(sample.x * mFaceCount - which,
			static_cast<real>(1) - std::numeric_limits<real>::epsilon()),
		sample.y);

	const auto positions = mesh::positions(which);

	// sample the triangle and get the point in barycentric coordinates
	const auto b = uniform_sample_triangle(sample_remapped);

	interaction interaction;

	interaction.point = positions[0] * b[0] + positions[1] * b[1] + positions[2] * (1 - b[0] - b[1]);
	interaction.normal = normalize(math::cross(positions[1] - positions[0], positions[2] - positions[0]));

	if (has_normal()) {
		const auto normals = mesh::normals(which);
		
		interaction.normal = normals[0] * b[0] + normals[1] * b[1] + normals[2] * (1 - b[0] - b[1]);
	}

	if (reverse_orientation()) interaction.normal = interaction.normal * -1.f;

	return shape_sample(
		interaction,
		pdf()
	);
}

rainbow::real rainbow::shapes::mesh::pdf() const
{
	return 1 / area();
}

rainbow::real rainbow::shapes::mesh::area() const noexcept
{
	return mArea;
}

rainbow::real rainbow::shapes::mesh::face_area(size_t face) const noexcept
{
	const auto points = positions(face);

	return static_cast<real>(0.5) * length(math::cross(points[2] - points[0], points[2] - points[1]));
}

std::array<rainbow::vector3, 3> rainbow::shapes::mesh::positions(size_t face) const noexcept
{
	return {
		mPositions[mIndices[face * 3 + 0]],
		mPositions[mIndices[face * 3 + 1]],
		mPositions[mIndices[face * 3 + 2]]
	};
}

std::array<rainbow::vector3, 3> rainbow::shapes::mesh::tangents(size_t face) const noexcept
{
	return {
		has_tangent() ? mTangents[mIndices[face * 3 + 0]] : vector3(),
		has_tangent() ? mTangents[mIndices[face * 3 + 1]] : vector3(),
		has_tangent() ? mTangents[mIndices[face * 3 + 2]] : vector3()
	};
}

std::array<rainbow::vector3, 3> rainbow::shapes::mesh::normals(size_t face) const noexcept
{
	return {
		has_normal() ? mNormals[mIndices[face * 3 + 0]] : vector3(),
		has_normal() ? mNormals[mIndices[face * 3 + 1]] : vector3(),
		has_normal() ? mNormals[mIndices[face * 3 + 2]] : vector3()
	};
}

std::array<rainbow::vector3, 3> rainbow::shapes::mesh::uvs(size_t face) const noexcept
{
	return {
		has_uv() ? mUVs[mIndices[face * 3 + 0]] : vector3(0, 0, 0),
		has_uv() ? mUVs[mIndices[face * 3 + 1]] : vector3(1, 0, 0),
		has_uv() ? mUVs[mIndices[face * 3 + 2]] : vector3(1, 1, 0)
	};
}

bool rainbow::shapes::mesh::has_tangent() const noexcept
{
	return !mTangents.empty();
}

bool rainbow::shapes::mesh::has_normal() const noexcept
{
	return !mNormals.empty();
}

bool rainbow::shapes::mesh::has_uv() const noexcept
{
	return !mUVs.empty();
}

std::shared_ptr<rainbow::shapes::mesh> rainbow::shapes::mesh::create_box(real width, real height, real depth, bool reverse_orientation)
{
	const auto w2 = 0.5f * width;
	const auto h2 = 0.5f * height;
	const auto d2 = 0.5f * depth;

	std::vector<vector3> positions = {
		vector3(-w2, -h2, -d2), vector3(-w2, +h2, -d2), vector3(+w2, +h2, -d2),
		vector3(+w2, -h2, -d2), vector3(-w2, -h2, +d2), vector3(+w2, -h2, +d2),
		vector3(+w2, +h2, +d2), vector3(-w2, +h2, +d2), vector3(-w2, +h2, -d2),
		vector3(-w2, +h2, +d2), vector3(+w2, +h2, +d2), vector3(+w2, +h2, -d2),
		vector3(-w2, -h2, -d2), vector3(+w2, -h2, -d2), vector3(+w2, -h2, +d2),
		vector3(-w2, -h2, +d2), vector3(-w2, -h2, +d2), vector3(-w2, +h2, +d2),
		vector3(-w2, +h2, -d2), vector3(-w2, -h2, -d2), vector3(+w2, -h2, -d2),
		vector3(+w2, +h2, -d2), vector3(+w2, +h2, +d2), vector3(+w2, -h2, +d2)
	};

	std::vector<vector3> tangents;
	
	std::vector<vector3> normals = {
		vector3(+0.0f, +0.0f, -1.0f), vector3(+0.0f, +0.0f, -1.0f), vector3(+0.0f, +0.0f, -1.0f),
		vector3(+0.0f, +0.0f, -1.0f), vector3(+0.0f, +0.0f, +1.0f), vector3(+0.0f, +0.0f, +1.0f),
		vector3(+0.0f, +0.0f, +1.0f), vector3(+0.0f, +0.0f, +1.0f), vector3(+0.0f, +1.0f, +0.0f),
		vector3(+0.0f, +1.0f, +0.0f), vector3(+0.0f, +1.0f, +0.0f), vector3(+0.0f, +1.0f, +0.0f),
		vector3(+0.0f, -1.0f, +0.0f), vector3(+0.0f, -1.0f, +0.0f), vector3(+0.0f, -1.0f, +0.0f),
		vector3(+0.0f, -1.0f, +0.0f), vector3(-1.0f, +0.0f, +0.0f), vector3(-1.0f, +0.0f, +0.0f),
		vector3(-1.0f, +0.0f, +0.0f), vector3(-1.0f, +0.0f, +0.0f), vector3(+1.0f, +0.0f, +0.0f),
		vector3(+1.0f, +0.0f, +0.0f), vector3(+1.0f, +0.0f, +0.0f), vector3(+1.0f, +0.0f, +0.0f)
	};

	std::vector<vector3> uvs = {
		vector3(+0.0f, +1.0f, +0.0f), vector3(+0.0f, +0.0f, +0.0f), vector3(+1.0f, +0.0f, +0.0f),
		vector3(+1.0f, +1.0f, +0.0f), vector3(+1.0f, +1.0f, +0.0f), vector3(+0.0f, +1.0f, +0.0f),
		vector3(+0.0f, +0.0f, +0.0f), vector3(+1.0f, +0.0f, +0.0f), vector3(+0.0f, +1.0f, +0.0f),
		vector3(+0.0f, +0.0f, +0.0f), vector3(+1.0f, +0.0f, +0.0f), vector3(+1.0f, +1.0f, +0.0f),
		vector3(+1.0f, +1.0f, +0.0f), vector3(+0.0f, +1.0f, +0.0f), vector3(+0.0f, +0.0f, +0.0f),
		vector3(+1.0f, +0.0f, +0.0f), vector3(+0.0f, +1.0f, +0.0f), vector3(+0.0f, +0.0f, +0.0f),
		vector3(+1.0f, +0.0f, +0.0f), vector3(+1.0f, +1.0f, +0.0f), vector3(+0.0f, +1.0f, +0.0f),
		vector3(+0.0f, +0.0f, +0.0f), vector3(+1.0f, +0.0f, +0.0f), vector3(+1.0f, +1.0f, +0.0f),
	};

	std::vector<unsigned> indices = {
		0, 1, 2, 0, 2, 3,
		4, 5 ,6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	return std::make_shared<mesh>(positions, tangents, normals, uvs, indices, reverse_orientation);
}

std::shared_ptr<rainbow::shapes::mesh> rainbow::shapes::mesh::create_quad(real width, real height, bool reverse_orientation)
{
	const auto w2 = 0.5f * width;
	const auto h2 = 0.5f * height;

	std::vector<vector3> positions = {
		vector3(-w2, +h2, 0), vector3(-w2, -h2, 0),
		vector3(+w2, -h2, 0), vector3(+w2, +h2, 0)
	};

	std::vector<vector3> tangents;
	
	std::vector<vector3> normals = {
		vector3(0, 0, 1), vector3(0, 0, 1),
		vector3(0, 0, 1), vector3(0, 0, 1)
	};

	std::vector<vector3> uvs = {
		vector3(0, 0, 0), vector3(0, 1, 0),
		vector3(1, 1, 0), vector3(1, 0, 0)
	};

	std::vector<unsigned> indices = {
		0, 1, 2,
		2, 3, 0
	};

	return std::make_shared<mesh>(positions, tangents, normals, uvs, indices, reverse_orientation);
}

std::optional<rainbow::surface_interaction> rainbow::shapes::mesh::intersect_with_triangle(const ray& ray, size_t face) const
{
	const auto positions = mesh::positions(face);
	const auto uvs = mesh::uvs(face);

	const auto e1 = positions[1] - positions[0];
	const auto e2 = positions[2] - positions[0];

	const auto p_vec = math::cross(ray.direction, e2);
	const auto t_vec = ray.origin - positions[0];
	const auto q_vec = math::cross(t_vec, e1);

	const auto inv_det = static_cast<real>(1) / dot(e1, p_vec);

	const auto b1 = dot(t_vec, p_vec) * inv_det;
	const auto b2 = dot(ray.direction, q_vec) * inv_det;
	const auto b0 = 1 - b1 - b2;

	const auto t = dot(e2, q_vec) * inv_det;

	if (t <= 0 || t >= ray.length) return std::nullopt;

	if (b0 < 0 || b0 > 1) return std::nullopt;
	if (b1 < 0 || b1 > 1) return std::nullopt;
	if (b2 < 0 || b2 > 1) return std::nullopt;

	const auto point = positions[0] * b0 + positions[1] * b1 + positions[2] * b2;
	const auto uv = uvs[0] * b0 + uvs[1] * b1 + uvs[2] * b2;
	const auto normal = reverse_orientation() ? -normalize(math::cross(e1, e2)) : normalize(math::cross(e1, e2));

	const auto duv02 = uvs[0] - uvs[2];
	const auto duv12 = uvs[1] - uvs[2];
	const auto dp02 = positions[0] - positions[2];
	const auto dp12 = positions[1] - positions[2];
	const auto determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
	const auto inv_determinant = 1 / determinant;

	auto dp_du = (+duv12[1] * dp02 - duv02[1] * dp12) * inv_determinant;
	auto dp_dv = (-duv12[0] * dp02 + duv02[0] * dp12) * inv_determinant;
	
	// when the uv is degenerate, we will use normal to generate a space
	if (abs(determinant) < 1e-8 || length(math::cross(dp_du, dp_dv)) == 0) {
		// the triangle is degenerate, we can not intersect it
		if (length_squared(normal) == 0) return std::nullopt;

		const auto system = coordinate_system(normal);

		dp_du = system.x();
		dp_dv = system.y();
	}

	// compute the shading space
	coordinate_system shading_space;

	// when we have normal property, we will use barycentric coordinates to compute the z of shading space
	// if we do not have it, we will use normal of triangle
	if (has_normal()) {
		// get normals
		const auto normals = mesh::normals(face);

		shading_space.z() = b0 * normals[0] + b1 * normals[1] + b2 * normals[2];

		// reverse the normal if need
		if (reverse_orientation()) shading_space.z() = shading_space.z() * -1.f;

		// when the normal is degenerate, we will use surface normal
		if (length_squared(shading_space.z()) <= 0) shading_space.z() = normal;
	}
	else shading_space.z() = normal;
	
	// when we have normal property, we will use barycentric coordinates to compute the x of shading space
	// if we do not have it, we will use dp_du
	if (has_tangent()) {
		// get tangents
		const auto tangents = mesh::tangents(face);

		shading_space.x() = b0 * tangents[0] + b1 * tangents[1] + b2 * tangents[2];

		// reverse the tangent if need
		if (reverse_orientation()) shading_space.x() = shading_space.x() * -1.f;

		// when the tangent is degenerate, we will use the dp_du
		if (length_squared(shading_space.x()) <= 0) shading_space.x() = dp_du;
	}
	else shading_space.x() = dp_du;

	shading_space.x() = normalize(shading_space.x());
	shading_space.z() = normalize(shading_space.z());
	
	// now, compute the "bitangent"(y) of shading space
	shading_space.y() = math::cross(shading_space.z(), shading_space.x());

	if (length_squared(shading_space.y()) > 0) {
		shading_space.y() = normalize(shading_space.y());
		shading_space.x() = normalize(math::cross(shading_space.y(), shading_space.z()));
	}
	else
		shading_space = coordinate_system(shading_space.z());

	ray.length = t;

	return surface_interaction(
		nullptr,
		shading_space,
		dp_du, dp_dv,
		normal, point, -ray.direction,
		uv
	);
}

#ifdef __GLTF_LOADER__

#define TINY_GLTF_HAS_VALUE(value) (value >= 0)

namespace rainbow {

	namespace shapes {

		template <typename T>
		void read_accessor(const tinygltf::Accessor* accessor, const tinygltf::Model* model, std::vector<T>& data)
		{
			const auto& bufferView = model->bufferViews[accessor->bufferView];
			const auto& buffer = model->buffers[bufferView.buffer];

			data = std::vector<T>(accessor->count);

			std::memcpy(data.data(), buffer.data.data() + bufferView.byteOffset + accessor->byteOffset,
				accessor->count * sizeof(T));
		}

		void read_unsigned_accessor(const tinygltf::Accessor* accessor, const tinygltf::Model* model, std::vector<unsigned>& data)
		{
			const auto& bufferView = model->bufferViews[accessor->bufferView];
			const auto& buffer = model->buffers[bufferView.buffer];

			if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) { read_accessor(accessor, model, data); return; }

			data = std::vector<unsigned>(accessor->count);

			if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
				std::vector<unsigned short> temp_data;

				read_accessor(accessor, model, temp_data);

				for (size_t index = 0; index < temp_data.size(); index++)
					data[index] = temp_data[index];
			}
		}
		
		void recursive_read_glb_mesh(const tinygltf::Model* model, const tinygltf::Node* node,
			std::vector<std::shared_ptr<mesh>>& meshes)
		{
			if (TINY_GLTF_HAS_VALUE(node->mesh)) {
				const auto& mesh = model->meshes[node->mesh];

				for (size_t index = 0; index < mesh.primitives.size(); index++) {
					const auto& primitives = mesh.primitives[index];

					std::vector<vector3> positions;
					std::vector<vector3> tangents;
					std::vector<vector3> normals;
					std::vector<vector3> uvs;

					std::vector<unsigned> indices;

					if (primitives.attributes.find("POSITION") != primitives.attributes.end())
						read_accessor(&model->accessors[primitives.attributes.at("POSITION")], model, positions);

					if (primitives.attributes.find("TANGENT") != primitives.attributes.end())
						read_accessor(&model->accessors[primitives.attributes.at("TANGENT")], model, tangents);

					if (primitives.attributes.find("NORMAL") != primitives.attributes.end())
						read_accessor(&model->accessors[primitives.attributes.at("NORMAL")], model, normals);

					if (primitives.attributes.find("TEXCOORD_0") != primitives.attributes.end())
						read_accessor(&model->accessors[primitives.attributes.at("TEXCOORD_0")], model, uvs);

					read_unsigned_accessor(&model->accessors[primitives.indices], model, indices);

					meshes.push_back(std::make_shared<shapes::mesh>(
						positions, tangents, normals, uvs, indices));
				}
			}
				
			for (const auto& child : node->children) 
				recursive_read_glb_mesh(model, &model->nodes[child], meshes);
		}
		
		std::vector<std::shared_ptr<mesh>> load_glb_mesh(const std::string& file)
		{
			tinygltf::Model model;
			std::string error;
			std::string warning;

			tinygltf::TinyGLTF loader;

			loader.LoadBinaryFromFile(&model, &error, &warning, file);

			std::vector<bool> is_root(model.nodes.size(), true);

			for (size_t index = 0; index < model.nodes.size(); index++) {
				for (const auto& child : model.nodes[index].children) {
					is_root[child] = false;
				}
			}

			std::vector<std::shared_ptr<mesh>> meshes;
			
			for (size_t index = 0; index < model.nodes.size(); index++) {
				if (!is_root[index]) continue;

				recursive_read_glb_mesh(&model, &model.nodes[index], meshes);
			}

			return meshes;
		}
		
	}
}

#endif