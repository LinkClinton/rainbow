#include "mesh.hpp"

#include "../shared/logs/log.hpp"

rainbow::shapes::mesh::mesh(
	const std::vector<vector3>& positions,
	const std::vector<vector3>& normals,
	const std::vector<vector3>& uvs,
	const std::vector<unsigned>& indices,
	bool reverse_orientation) : shape(reverse_orientation),
	mPositions(positions), mNormals(normals), mUVs(uvs),
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
	throw std::exception("not implementation.");
}

rainbow::real rainbow::shapes::mesh::pdf() const
{
	throw std::exception("not implementation.");
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

	return std::make_shared<mesh>(positions, normals, uvs, indices, reverse_orientation);
}

std::shared_ptr<rainbow::shapes::mesh> rainbow::shapes::mesh::create_quad(real width, real height, bool reverse_orientation)
{
	const auto w2 = 0.5f * width;
	const auto h2 = 0.5f * height;

	std::vector<vector3> positions = {
		vector3(-w2, +h2, 0), vector3(-w2, -h2, 0),
		vector3(+w2, -h2, 0), vector3(+w2, +h2, 0)
	};

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

	return std::make_shared<mesh>(positions, normals, uvs, indices, reverse_orientation);
}

std::optional<rainbow::surface_interaction> rainbow::shapes::mesh::intersect_with_triangle(const ray& ray, size_t face) const
{
	const auto points = positions(face);
	const auto uvs = mesh::uvs(face);
	
	const auto e1 = points[1] - points[0];
	const auto e2 = points[2] - points[0];

	const auto p_vec = math::cross(ray.direction, e2);
	const auto t_vec = ray.origin - points[0];
	const auto q_vec = math::cross(t_vec, e1);
	
	const auto inv_det = static_cast<real>(1) / dot(e1, p_vec);
	
	const auto b1 = dot(t_vec, p_vec) * inv_det;
	const auto b2 = dot(ray.direction, q_vec) * inv_det;
	const auto b0 = 1 - b1 - b2;

	const auto t = dot(e2, q_vec) * inv_det;

	if (t <= 0 || t >= ray.length) return {};

	if (b0 < 0 || b0 > 1) return {};
	if (b1 < 0 || b1 > 1) return {};
	if (b2 < 0 || b2 > 1) return {};

	const auto point = points[0] * b0 + points[1] * b1 + points[2] * b2;
	const auto uv = uvs[0] * b0 + uvs[1] * b1 + uvs[2] * b2;
	const auto normal = reverse_orientation() ? -normalize(math::cross(e1, e2)) : normalize(math::cross(e1, e2));
	const auto system = coordinate_system(normal);

	ray.length = t;
	
	return surface_interaction(
		nullptr,
		system.x(), system.y(),
		normal, point, -ray.direction,
		uv
	);
}

