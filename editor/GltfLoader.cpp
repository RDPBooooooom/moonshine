//
// Created by marvin on 16.11.2023.
//

#include "GltfLoader.h"
#include "Scene.h"

namespace moonshine {
    int GltfLoader::nr_loaded_objects = 0;

    std::vector<std::shared_ptr<SceneObject>> GltfLoader::load_gltf(std::string filepath, std::string filename, boost::uuids::uuid uuid) {

        tinygltf::Model m_model;
        std::vector<std::shared_ptr<SceneObject>> loadedObjects;

        ModelLoader::loadASCIIModel(m_model, (filepath + filename).c_str());

        for (const auto &scene: m_model.scenes) {
            for (const auto &nodeIndex: scene.nodes) {
                std::vector<std::shared_ptr<Node>> nodeList;
                nr_loaded_objects++;

                auto node = m_model.nodes[nodeIndex];

                nodeList = getSubmeshes(m_model, node, nullptr, filepath);

                std::string name;
                if (node.name.empty()) {
                    name = "?";
                } else {
                    name = node.name;
                }
                
                std::shared_ptr<SceneObject> object;
                if(uuid.is_nil()){
                    object = std::make_shared<SceneObject>(name, nodeList);
                } else{
                    object = std::make_shared<SceneObject>(name, nodeList, uuid);
                }
                
                loadedObjects.push_back(object);
            }
        }

        return loadedObjects;
    }

    std::vector<std::shared_ptr<Node>>
    GltfLoader::getSubmeshes(tinygltf::Model &model, tinygltf::Node &node, std::shared_ptr<Node> parent,
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

        std::shared_ptr<Node> moonshineNode = std::make_shared<Node>(parent, transform);
        std::vector<std::shared_ptr<Node>> nodes;
        nodes.push_back(moonshineNode);

        for (const auto &item: node.children) {
            std::vector<std::shared_ptr<Node>> subMeshes = getSubmeshes(model, model.nodes[item], moonshineNode, path);
            nodes.insert(nodes.end(), std::make_move_iterator(subMeshes.begin()),
                         std::make_move_iterator(subMeshes.end()));
        }

        if (node.mesh < 0) return nodes;

        auto mesh = model.meshes[node.mesh];
        for (const auto &primitive: mesh.primitives) {
            GltfData data = {};

            // Indices 
            {
                const auto &accessor = model.accessors[primitive.indices];
                const auto &bufferView = model.bufferViews[accessor.bufferView];
                const auto &buffer = model.buffers[bufferView.buffer];
                const size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                const size_t stride = bufferView.byteStride ? bufferView.byteStride : sizeof(float);
                const uint8_t *indices = &buffer.data[byteOffset];

                if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE) {
                    // handle indices as unsigned bytes
                    const auto *indices_uint8 = reinterpret_cast<const uint8_t *>(indices);
                    std::cout << "[Indices Import] Unsigned byte - Currently not supported - Fix it Marvin! \n";
                    // ... use indices_uint8 ...
                } else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
                    // handle indices as unsigned shorts

                    const auto *indices_uint16 = reinterpret_cast<const uint16_t *>(indices);
                    data.m_indices.resize(accessor.count);

                    for (size_t indiceIndex = 0; indiceIndex < accessor.count; ++indiceIndex) {

                        size_t bufferIndex = indiceIndex * stride / sizeof(float);

                        data.m_indices[indiceIndex] = indices_uint16[bufferIndex];
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
                const auto &bufferView = model.bufferViews[accessor.bufferView];
                const auto &buffer = model.buffers[bufferView.buffer];

                if (data.m_vertices.empty()) {
                    data.m_vertices.resize(accessor.count);
                }

                size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;

                if (std::equal(attributes.first.begin(), attributes.first.end(), "POSITION")) {
                    // Check if accessor type and component type are correct for a vertex position (should be a vector of 3 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC3 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        // Get the byte offset and size of the POSITION data

                        size_t stride = bufferView.byteStride ? bufferView.byteStride : sizeof(float) * 3;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *positions = reinterpret_cast<const float *>(&buffer.data[byteOffset]);

                        for (size_t vertexIndex = 0; vertexIndex < accessor.count; ++vertexIndex) {
                            // Calculate the index into the bufferFloats array
                            size_t bufferIndex = vertexIndex * stride / sizeof(float);

                            // Access the vertex position data
                            glm::vec3 pos = glm::vec3(positions[bufferIndex],
                                                      positions[bufferIndex + 1],
                                                      positions[bufferIndex + 2]);

                            // Assign the position to the vertex in your mesh data structure
                            data.m_vertices[vertexIndex].pos = pos;
                        }
                    }
                } else if (std::equal(attributes.first.begin(), attributes.first.end(), "NORMAL")) {
                    // Check if accessor type and component type are correct for a normal (should be a vector of 3 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC3 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {

                        size_t stride = bufferView.byteStride ? bufferView.byteStride : sizeof(float) * 3;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *normals = reinterpret_cast<const float *>(&buffer.data[byteOffset]);

                        // Each normal is a vec3, so there are 3 floats per normal
                        for (size_t normalIndex = 0; normalIndex < accessor.count; ++normalIndex) {

                            size_t bufferIndex = normalIndex * stride / sizeof(float);

                            glm::vec3 normal(normals[bufferIndex],
                                             normals[bufferIndex + 1],
                                             normals[bufferIndex + 2]);
                            data.m_vertices[normalIndex].normal = normal;
                        }
                    }
                } else if (std::equal(attributes.first.begin(), attributes.first.end(), "TEXCOORD_0")) {
                    // Check if accessor type and component type are correct for an uv (should be a vector of 2 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC2 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {

                        size_t stride = bufferView.byteStride ? bufferView.byteStride : sizeof(float) * 2;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *uvs = reinterpret_cast<const float *>(&buffer.data[byteOffset]);

                        // Each normal is a vec3, so there are 3 floats per normal
                        for (size_t uvIndex = 0; uvIndex < accessor.count; ++uvIndex) {

                            size_t bufferIndex = uvIndex * stride / sizeof(float);

                            glm::vec2 uv(uvs[bufferIndex],
                                         uvs[bufferIndex + 1]);
                            data.m_vertices[uvIndex].texCoord = uv;
                        }
                    }
                }
            }

            if (primitive.material > -1) {
                auto mat = model.materials[primitive.material];
                int textureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;

                if (mat.name.empty()) {
                    data.m_matName = "Mat: " + std::to_string(primitive.material);
                } else {
                    data.m_matName = mat.name;
                }
                
                if(textureIndex < 0){
                    throw std::runtime_error("invalid texture index for pbrMetallicRoughness.baseColorTexture");
                }

                data.m_texName = model.images[textureIndex].uri;
                data.m_path = path;
            }
            data.m_name = mesh.name;
            meshes.push_back(data);
        }

        moonshineNode->set_gltf_data(meshes);

        return nodes;
    }
} // moonshine