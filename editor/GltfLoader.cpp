//
// Created by marvin on 16.11.2023.
//

#include "GltfLoader.h"
#include "Scene.h"

namespace moonshine {
    int GltfLoader::nr_loaded_objects = 0;

    std::vector<std::shared_ptr<SceneObject>> GltfLoader::load_gltf(std::string filepath, std::string filename) {

        tinygltf::Model m_model;
        std::vector<std::shared_ptr<SceneObject>> loadedObjects;

        ModelLoader::loadASCIIModel(m_model, (filepath + filename).c_str());

        for (const auto &scene: m_model.scenes) {
            for (const auto &nodeIndex: scene.nodes) {
                std::cout << "Node: " << nodeIndex;
                std::vector<std::shared_ptr<Node>> nodeList;
                nr_loaded_objects++;
                
                auto node = m_model.nodes[nodeIndex];

                nodeList = getSubmeshes(m_model, node, nullptr, filepath);

                std::string name;
                if (node.name.empty()) {
                    name = "Obj: " + std::to_string(nr_loaded_objects);
                } else {
                    name = node.name;
                }

                auto object = std::make_shared<SceneObject>(name, nodeList);
                loadedObjects.push_back(object);
            }
        }
        
        return loadedObjects;
    }

    std::vector<std::shared_ptr<Node>> GltfLoader::getSubmeshes(tinygltf::Model &model, tinygltf::Node &node, std::shared_ptr<Node> parent, std::string &path) {
        
        std::cout << "get mesh for node: " << node.name << std::endl;
        std::vector<GltfData> meshes;

        Transform transform = {};
        if(!node.translation.empty()){
            transform.position = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
        }
        if(!node.rotation.empty()){
            transform.rotation = glm::quat(glm::vec3(node.rotation[0], node.rotation[1], node.rotation[2]));
        }
        if(!node.scale.empty()){
            transform.scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
        }

        std::shared_ptr<Node> moonshineNode = std::make_shared<Node>(parent, transform);
        std::vector<std::shared_ptr<Node>> nodes;
        nodes.push_back(moonshineNode);

        for (const auto &item: node.children){
            std::cout << "Child => ";
            std::vector<std::shared_ptr<Node>> subMeshes = getSubmeshes(model, model.nodes[item], moonshineNode, path);
            nodes.insert(nodes.end(), std::make_move_iterator(subMeshes.begin()), std::make_move_iterator(subMeshes.end()));
        }
        
        if(node.mesh < 0) return nodes;
        
        auto mesh = model.meshes[node.mesh];
        for (const auto &primitive: mesh.primitives) {
            std::cout << "Primitive (via indices: " << primitive.indices;
            GltfData data = {};
            
            const auto &accessor = model.accessors[primitive.indices];
            const auto &bufferView = model.bufferViews[accessor.bufferView];
            const auto &buffer = model.buffers[bufferView.buffer];
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
                std::cout << "indices size: " << std::to_string(data.m_indices.size()) << std::endl;
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

            for (const auto &attributes: primitive.attributes) {
                if (std::equal(attributes.first.begin(), attributes.first.end(), "POSITION")) {
                    const auto &accessor = model.accessors[attributes.second];
                    const auto &bufferView = model.bufferViews[accessor.bufferView];
                    const auto &buffer = model.buffers[bufferView.buffer];

                    // Check if accessor type and component type are correct for a vertex position (should be a vector of 3 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC3 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        // Get the byte offset and size of the POSITION data
                        size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                        size_t byteLength = bufferView.byteLength;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *positions = reinterpret_cast<const float *>(&buffer.data[byteOffset]);

                        if (data.m_vertices.empty()) {
                            std::cout << "vertices size (by position): " << std::to_string(data.m_indices.size()) << std::endl;
                            data.m_vertices.resize(byteLength / sizeof(float) / 3);
                        }

                        for (size_t i = 0; i < byteLength / sizeof(float); i += 3) {
                            glm::vec3 pos = glm::vec3(positions[i], positions[i + 1], positions[i + 2]);
                            data.m_vertices[i / 3].pos = pos;
                        }
                    }
                } else if (std::equal(attributes.first.begin(), attributes.first.end(), "NORMAL")) {
                    const auto &accessor = model.accessors[attributes.second];
                    const auto &bufferView = model.bufferViews[accessor.bufferView];
                    const auto &buffer = model.buffers[bufferView.buffer];

                    // Check if accessor type and component type are correct for a normal (should be a vector of 3 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC3 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        // Get the byte offset and size of the POSITION data
                        size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                        size_t byteLength = bufferView.byteLength;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *normals = reinterpret_cast<const float *>(&buffer.data[byteOffset]);

                        if (data.m_vertices.empty()) {
                            std::cout << "vertices size (by normals): " << std::to_string(data.m_indices.size()) << std::endl;
                            data.m_vertices.resize(byteLength / sizeof(float) / 3);
                        }

                        // Each normal is a vec3, so there are 3 floats per normal
                        for (size_t i = 0; i < accessor.count; ++i) {
                            glm::vec3 normal(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
                            data.m_vertices[i].normal = normal;
                        }
                    }
                } else if (std::equal(attributes.first.begin(), attributes.first.end(), "TEXCOORD_0")) {
                    const auto &accessor = model.accessors[attributes.second];
                    const auto &bufferView = model.bufferViews[accessor.bufferView];
                    const auto &buffer = model.buffers[bufferView.buffer];

                    // Check if accessor type and component type are correct for an uv (should be a vector of 2 floats)
                    if (accessor.type == TINYGLTF_TYPE_VEC2 &&
                        accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        // Get the byte offset and size of the POSITION data
                        size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                        size_t byteLength = bufferView.byteLength;

                        // Cast the raw buffer data to float and print the vertex positions
                        const auto *uvs = reinterpret_cast<const float *>(&buffer.data[byteOffset]);

                        if (data.m_vertices.empty()) {
                            std::cout << "vertices size (by uv): " << std::to_string(data.m_indices.size()) << std::endl;
                            data.m_vertices.resize(accessor.count);
                        }

                        // Each normal is a vec3, so there are 3 floats per normal
                        for (size_t i = 0; i < accessor.count; ++i) {
                            glm::vec2 uv(uvs[i * 2], uvs[i * 2 + 1]);
                            data.m_vertices[i].texCoord = uv;
                        }
                    }
                }
            }

            if (primitive.material > -1) {
                std::cout << "access material with index:" << std::to_string(primitive.material) << std::endl;
                auto mat = model.materials[primitive.material];
                int textureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;

                if (mat.name.empty()) {
                    data.m_matName = "Mat: " + std::to_string(primitive.material);
                } else {
                    data.m_matName = mat.name;
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