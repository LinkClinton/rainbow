#include "mesh.hpp"

rainbow::shapes::mesh::mesh(
	const std::vector<vector3>& positions,
	const std::vector<vector3>& normals,
	const std::vector<vector3>& uvs,
	const std::vector<unsigned>& indices) :
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

	return 0.5 * length(math::cross(points[2] - points[0], points[2] - points[1]));
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

std::optional<rainbow::surface_interaction> rainbow::shapes::mesh::intersect_with_triangle(const ray& ray, size_t face) const
{
	const auto points = positions(face);

	
}

