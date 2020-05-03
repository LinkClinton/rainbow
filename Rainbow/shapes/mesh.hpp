#pragma once

#include "shape.hpp"

#include <vector>
#include <array>

#define __GLTF_LOADER__

namespace rainbow {

	namespace shapes {
		
		class mesh final : public shape {
		public:
			explicit mesh(
				const std::vector<vector3>& positions,
				const std::vector<vector3>& tangents,
				const std::vector<vector3>& normals,
				const std::vector<vector3>& uvs,
				const std::vector<unsigned>& indices,
				bool reverse_orientation = false);

			~mesh() = default;

			std::optional<surface_interaction> intersect(const ray& ray) const override;

			shape_sample sample(const vector2& sample) const override;

			real pdf() const override;

			real area() const noexcept override;

			real face_area(size_t face) const noexcept;

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
			std::optional<surface_interaction> intersect_with_triangle(const ray& ray, size_t face) const;
			
			std::vector<vector3> mPositions;
			std::vector<vector3> mTangents;
			std::vector<vector3> mNormals;
			std::vector<vector3> mUVs;
			
			std::vector<unsigned> mIndices;

			size_t mFaceCount;
			
			real mArea;
		};

#ifdef __GLTF_LOADER__
		std::vector<std::shared_ptr<mesh>> load_glb_mesh(const std::string& file);
#endif
	}
}
