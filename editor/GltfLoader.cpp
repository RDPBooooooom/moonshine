//
// Created by marvin on 16.11.2023.
//

#include "GltfLoader.h"
#include "ModelLoader.h"
#include "Scene.h"

namespace moonshine {
    int GltfLoader::nr_loaded_objects = 0;
    
    std::vector<std::shared_ptr<SceneObject>> GltfLoader::load_gltf(std::string filepath, std::string filename) {

        tinygltf::Model m_model;
        GltfData data = {};
        std::vector<std::shared_ptr<SceneObject>> loadedObjects;

        ModelLoader::loadASCIIModel(m_model, (filepath + filename).c_str());

        // Iterate over all meshes in the model
        for (const auto &mesh: m_model.meshes) {
            // Iterate over all primitives in the mesh
            for (const auto &primitive: mesh.primitives) {
                nr_loaded_objects++;
                // Check if the primitive has POSITION attribute
                if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                    const auto &accessor = m_model.accessors[primitive.attributes.at("POSITION")];
                    const auto &bufferView = m_model.bufferViews[accessor.bufferView];
                    const auto &buffer = m_model.buffers[bufferView.buffer];

                    // Check if accessor type and component type are correct for a vertex position (should be a vector of 3 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC3 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        // Get the byte offset and size of the POSITION data
                        size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                        size_t byteLength = bufferView.byteLength;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *positions = reinterpret_cast<const float *>(&buffer.data[byteOffset]);
                        data.m_vertices.resize(byteLength / sizeof(float) / 3);
                        for (size_t i = 0; i < byteLength / sizeof(float); i += 3) {
                            auto vertex = Vertex{};
                            //std::cout << "Vertex position: (" << positions[i] << ", " << positions[i + 1] << ", " << positions[i + 2] << ")\n";
                            vertex.pos = glm::vec3(positions[i], positions[i + 1], positions[i + 2]);
                            data.m_vertices[i / 3] = (vertex);
                        }
                    }
                }
                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                    const auto &accessor = m_model.accessors[primitive.attributes.at("NORMAL")];
                    const auto &bufferView = m_model.bufferViews[accessor.bufferView];
                    const auto &buffer = m_model.buffers[bufferView.buffer];

                    // Check if accessor type and component type are correct for a vertex position (should be a vector of 3 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC3 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        // Get the byte offset and size of the POSITION data
                        size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                        size_t byteLength = bufferView.byteLength;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *normals = reinterpret_cast<const float *>(&buffer.data[byteOffset]);
                        // Each normal is a vec3, so there are 3 floats per normal
                        for (size_t i = 0; i < accessor.count; ++i) {
                            glm::vec3 normal(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
                            //std::cout << "Normals position: (" << normal.x << ", " << normal.y << ", " << normal.z << ")\n";
                            data.m_vertices[i].normal = normal;
                        }
                    }
                }
                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                    const auto &accessor = m_model.accessors[primitive.attributes.at("TEXCOORD_0")];
                    const auto &bufferView = m_model.bufferViews[accessor.bufferView];
                    const auto &buffer = m_model.buffers[bufferView.buffer];

                    // Check if accessor type and component type are correct for a vertex position (should be a vector of 3 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC2 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        // Get the byte offset and size of the POSITION data
                        size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                        size_t byteLength = bufferView.byteLength;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *normals = reinterpret_cast<const float *>(&buffer.data[byteOffset]);
                        // Each normal is a vec3, so there are 3 floats per normal
                        for (size_t i = 0; i < accessor.count; ++i) {
                            glm::vec2 uv(normals[i * 2], normals[i * 2 + 1]);
                            data.m_vertices[i].texCoord = uv;
                        }
                    }
                }
                if (primitive.indices > -1) {
                    const tinygltf::Accessor &accessor = m_model.accessors[primitive.indices];
                    const tinygltf::BufferView &bufferView = m_model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer &buffer = m_model.buffers[bufferView.buffer];

                    const size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                    const uint8_t *indices = &buffer.data[byteOffset];

                    if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE) {
                        // handle indices as unsigned bytes
                        const auto *indices_uint8 = reinterpret_cast<const uint8_t *>(indices);
                        //std::cout << "[Indices Import] Unsigned byte - Currently not supported - Fix it Marvin! \n";
                        // ... use indices_uint8 ...
                    } else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
                        // handle indices as unsigned shorts
                        size_t byteLength = bufferView.byteLength;

                        const auto *indices_uint16 = reinterpret_cast<const uint16_t *>(indices);
                        data.m_indices.resize(byteLength / sizeof(uint16_t));
                        for (size_t i = 0; i < byteLength / sizeof(uint16_t); ++i) {
                            //std::cout << "Index: (" << indices_uint16[i] << ")\n";
                            data.m_indices[i] = indices_uint16[i];
                        }
                    } else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
                        // handle indices as unsigned ints
                        const auto *indices_uint32 = reinterpret_cast<const uint32_t *>(indices);
                        //std::cout << "[Indices Import] Unsigned int - Currently not supported - Fix it Marvin! \n";
                        // ... use indices_uint32 ...
                    }
                }
                if (primitive.material > -1) {

                    auto mat = m_model.materials[primitive.material];
                    int textureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;

                    if (mat.name.empty()) {
                        data.m_matName = "Mat: " + std::to_string(primitive.material);
                    } else {
                        data.m_matName = mat.name;
                    }

                    data.m_texName = m_model.images[textureIndex].uri;
                    data.m_path = filepath;
                }

                if (mesh.name.empty()) {
                    data.m_name = "Obj: " + std::to_string(nr_loaded_objects);
                } else {
                    data.m_name = mesh.name;
                }

                auto object = std::make_shared<SceneObject>(data);
                loadedObjects.push_back(object);
            }
        }


        return loadedObjects;
    }
} // moonshine