//
// Created by marvin on 16.11.2023.
//

#include <filesystem>
#include "GltfLoader.h"
#include "Scene.h"
#include "EngineSystems.h"

namespace moonshine {
    int GltfLoader::s_nr_loaded_objects = 0;

    std::vector<std::shared_ptr<SceneObject>>
    GltfLoader::load_gltf(std::string &filepath, std::string &filename, boost::uuids::uuid &uuid) {

        tinygltf::Model model;
        std::vector<std::shared_ptr<SceneObject>> loaded_objects;

        ModelLoader::load_ascii_model(model, (filepath + filename).c_str());

        for (const auto &scene: model.scenes) {
            for (const auto &node_index: scene.nodes) {
                std::vector<std::shared_ptr<Node>> node_list;
                s_nr_loaded_objects++;

                auto node = model.nodes[node_index];

                node_list = get_submeshes(model, node, nullptr, filepath);

                std::string name;
                if (node.name.empty()) {
                    name = "?";
                } else {
                    name = node.name;
                }

                std::shared_ptr<SceneObject> object;
                if (uuid.is_nil()) {
                    object = std::make_shared<SceneObject>(name, node_list);
                } else {
                    object = std::make_shared<SceneObject>(name, node_list, uuid);
                }

                SceneObject::object_meta_data meta_data = {};

                std::filesystem::path path(filepath);

                meta_data.path = path.parent_path().stem().string() + "\\";
                meta_data.filename = filename;
                object->set_meta_data(meta_data);

                loaded_objects.push_back(object);
            }
        }

        if (loaded_objects.size() > 1) {
            // Since the meta_data is contained in each object its problematic if such a scene gets saved and loaded. For each object that's in the scene each model in the file gets loaded again!
            EngineSystems::get_instance().get_logger()->warn(LoggerType::Editor,
                                                             "Its currently not supported to load gltf files that contain multiple models. This leads to problems when saving and loading the scene.");
        }

        return loaded_objects;
    }

    std::vector<std::shared_ptr<Node>>
    GltfLoader::get_submeshes(tinygltf::Model &model, tinygltf::Node &node, std::shared_ptr<Node> parent,
                              std::string &path) {
        std::vector<GltfData> meshes;

        Transform transform = {};
        if (!node.translation.empty()) {
            transform.position = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
        }
        if (!node.rotation.empty()) {
            transform.rotation = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
        }
        if (!node.scale.empty()) {
            transform.scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
        }

        std::shared_ptr<Node> moonshine_node = std::make_shared<Node>(parent, transform);
        std::vector<std::shared_ptr<Node>> nodes;
        nodes.push_back(moonshine_node);

        for (const auto &item: node.children) {
            std::vector<std::shared_ptr<Node>> submeshes = get_submeshes(model, model.nodes[item], moonshine_node,
                                                                         path);
            nodes.insert(nodes.end(), std::make_move_iterator(submeshes.begin()),
                         std::make_move_iterator(submeshes.end()));
        }

        if (node.mesh < 0) return nodes;

        auto mesh = model.meshes[node.mesh];
        for (const auto &primitive: mesh.primitives) {
            GltfData data = {};

            // Indices 
            {
                const auto &accessor = model.accessors[primitive.indices];
                const auto &buffer_view = model.bufferViews[accessor.bufferView];
                const auto &buffer = model.buffers[buffer_view.buffer];
                const size_t byte_offset = accessor.byteOffset + buffer_view.byteOffset;
                const size_t stride = buffer_view.byteStride ? buffer_view.byteStride : sizeof(float);
                const uint8_t *indices = &buffer.data[byte_offset];

                if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE) {
                    // handle indices as unsigned bytes
                    const auto *indices_uint8 = reinterpret_cast<const uint8_t *>(indices);
                    std::cout << "[Indices Import] Unsigned byte - Currently not supported - Fix it Marvin! \n";
                    // ... use indices_uint8 ...
                } else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
                    // handle indices as unsigned shorts

                    const auto *indices_uint16 = reinterpret_cast<const uint16_t *>(indices);
                    data.m_indices.resize(accessor.count);

                    for (size_t indice_index = 0; indice_index < accessor.count; ++indice_index) {

                        size_t buffer_index = indice_index * stride / sizeof(float);

                        data.m_indices[indice_index] = indices_uint16[buffer_index];
                    }
                } else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
                    // handle indices as unsigned ints
                    const auto *indices_uint32 = reinterpret_cast<const uint32_t *>(indices);
                    std::cout << "[Indices Import] Unsigned int - Currently not supported - Fix it Marvin! \n";
                    // ... use indices_uint32 ...
                }
            }

            for (const auto &attributes: primitive.attributes) {

                const auto &accessor = model.accessors[attributes.second];
                const auto &buffer_view = model.bufferViews[accessor.bufferView];
                const auto &buffer = model.buffers[buffer_view.buffer];

                if (data.m_vertices.empty()) {
                    data.m_vertices.resize(accessor.count);
                }

                size_t byte_offset = accessor.byteOffset + buffer_view.byteOffset;

                if (std::equal(attributes.first.begin(), attributes.first.end(), "POSITION")) {
                    // Check if accessor type and component type are correct for a vertex position (should be a vector of 3 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC3 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        // Get the byte offset and size of the POSITION data

                        size_t stride = buffer_view.byteStride ? buffer_view.byteStride : sizeof(float) * 3;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *positions = reinterpret_cast<const float *>(&buffer.data[byte_offset]);

                        for (size_t vertex_index = 0; vertex_index < accessor.count; ++vertex_index) {
                            // Calculate the index into the bufferFloats array
                            size_t buffer_index = vertex_index * stride / sizeof(float);

                            // Access the vertex position data
                            glm::vec3 pos = glm::vec3(positions[buffer_index],
                                                      positions[buffer_index + 1],
                                                      positions[buffer_index + 2]);

                            // Assign the position to the vertex in your mesh data structure
                            data.m_vertices[vertex_index].pos = pos;
                        }
                    }
                } else if (std::equal(attributes.first.begin(), attributes.first.end(), "NORMAL")) {
                    // Check if accessor type and component type are correct for a normal (should be a vector of 3 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC3 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {

                        size_t stride = buffer_view.byteStride ? buffer_view.byteStride : sizeof(float) * 3;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *normals = reinterpret_cast<const float *>(&buffer.data[byte_offset]);

                        // Each normal is a vec3, so there are 3 floats per normal
                        for (size_t normal_index = 0; normal_index < accessor.count; ++normal_index) {

                            size_t buffer_index = normal_index * stride / sizeof(float);

                            glm::vec3 normal(normals[buffer_index],
                                             normals[buffer_index + 1],
                                             normals[buffer_index + 2]);
                            data.m_vertices[normal_index].normal = normal;
                        }
                    }
                } else if (std::equal(attributes.first.begin(), attributes.first.end(), "TEXCOORD_0")) {
                    // Check if accessor type and component type are correct for an uv (should be a vector of 2 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC2 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {

                        size_t stride = buffer_view.byteStride ? buffer_view.byteStride : sizeof(float) * 2;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *uvs = reinterpret_cast<const float *>(&buffer.data[byte_offset]);

                        // Each normal is a vec3, so there are 3 floats per normal
                        for (size_t uv_index = 0; uv_index < accessor.count; ++uv_index) {

                            size_t buffer_index = uv_index * stride / sizeof(float);

                            glm::vec2 uv(uvs[buffer_index],
                                         uvs[buffer_index + 1]);
                            data.m_vertices[uv_index].texCoord = uv;
                        }
                    }
                }
            }

            if (primitive.material > -1) {
                auto mat = model.materials[primitive.material];
                int texture_index = mat.pbrMetallicRoughness.baseColorTexture.index;

                if (mat.name.empty()) {
                    data.m_matName = "Mat: " + std::to_string(primitive.material);
                } else {
                    data.m_matName = mat.name;
                }

                if (texture_index < 0) {
                    throw std::runtime_error("invalid texture index for pbrMetallicRoughness.baseColorTexture");
                }

                data.m_texName = model.images[texture_index].uri;
                data.m_path = path;
            }
            data.m_name = mesh.name;
            meshes.push_back(data);
        }

        moonshine_node->set_gltf_data(meshes);

        return nodes;
    }
} // moonshine