#include "gltf_loader.hpp"

#ifdef __GLTF_LOADER__

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_JSON

#include <nlohmann/json.hpp>
#include <tiny_gltf.h>

#endif

#ifdef __GLTF_LOADER__

#define TINY_GLTF_HAS_VALUE(value) (value >= 0)

namespace rainbow {

	namespace scenes {

		namespace loaders {

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

			std::vector<std::shared_ptr<mesh>> load_glb_mesh(const std::string& file_name)
			{
				tinygltf::Model model;
				std::string error;
				std::string warning;

				tinygltf::TinyGLTF loader;

				loader.LoadBinaryFromFile(&model, &error, &warning, file_name);

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
}

#endif