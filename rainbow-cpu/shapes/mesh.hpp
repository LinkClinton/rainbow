#pragma once

#include "../shared/accelerators/accelerator.hpp"
#include "../textures/texture.hpp"

#include "shape.hpp"

#include <vector>
#include <array>

namespace rainbow::cpus::shapes {

	using namespace accelerators;
	using namespace textures;

	class mesh final : public shape {
	public:
		explicit mesh(
			const std::vector<vector3>& positions,
			const std::vector<vector3>& tangents,
			const std::vector<vector3>& normals,
			const std::vector<vector3>& uvs,
			const std::vector<unsigned>& indices,
			bool reverse_orientation = false);

		explicit mesh(
			const std::shared_ptr<texture2d<real>>& mask,
			const std::vector<vector3>& positions,
			const std::vector<vector3>& tangents,
			const std::vector<vector3>& normals,
			const std::vector<vector3>& uvs,
			const std::vector<unsigned>& indices,
			bool reverse_orientation = false);

		~mesh() = default;

		std::optional<surface_interaction> intersect(const ray& ray, size_t index) const override;

		std::optional<surface_interaction> intersect(const ray& ray) const override;

		bound3 bounding_box(const transform& transform, size_t index) const override;

		bound3 bounding_box(const transform& transform) const override;

		shape_sample sample(const shape_instance_properties& properties, const vector2& sample) const override;

		real pdf(const shape_instance_properties& properties) const override;

		real area(const transform& transform, size_t index) const noexcept override;
		
		real area(const transform& transform) const noexcept override;
		
		real area(size_t index) const noexcept override;

		real area() const noexcept override;

		void build_accelerator() override;

		std::array<vector3, 3> positions(size_t face) const noexcept;

		std::array<vector3, 3> tangents(size_t face) const noexcept;

		std::array<vector3, 3> normals(size_t face) const noexcept;

		std::array<vector3, 3> uvs(size_t face) const noexcept;

		bool has_tangent() const noexcept;

		bool has_normal() const noexcept;

		bool has_uv() const noexcept;

		static std::shared_ptr<mesh> create_box(real width, real height, real depth, bool reverse_orientation = false);

		static std::shared_ptr<mesh> create_quad(real width, real height, bool reverse_orientation = false);
	private:
		struct mesh_reference {
			mesh* const instance = nullptr;

			size_t face = 0;

			mesh_reference() = default;

			mesh_reference(mesh* const instance, size_t face);

			std::optional<surface_interaction> intersect(const ray& ray) const;

			bound3 bounding_box() const;

			bool visible() const noexcept;
		};

		std::optional<surface_interaction> intersect_with_triangle(const ray& ray, size_t face) const;
	private:
		std::shared_ptr<accelerator<mesh_reference>> mAccelerator;
		std::shared_ptr<texture2d<real>> mMask;

		std::vector<vector3> mPositions;
		std::vector<vector3> mTangents;
		std::vector<vector3> mNormals;
		std::vector<vector3> mUVs;

		std::vector<unsigned> mIndices;

		bound3 mBoundingBox;

		real mArea;
	};
}