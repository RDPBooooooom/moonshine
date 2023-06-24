//
// Created by marvin on 22.06.2023.
//

#include "SceneObject.h"
#include "ModelLoader.h"

moonshine::SceneObject::SceneObject(const char *filepath) {
    ModelLoader::loadASCIIModel(model, filepath);

    // Iterate over all meshes in the model
    for (const auto& mesh : model.meshes) {
        // Iterate over all primitives in the mesh
        for (const auto& primitive : mesh.primitives) {
            // Check if the primitive has POSITION attribute
            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const auto& accessor = model.accessors[primitive.attributes.at("POSITION")];
                const auto& bufferView = model.bufferViews[accessor.bufferView];
                const auto& buffer = model.buffers[bufferView.buffer];

                // Check if accessor type and component type are correct for a vertex position (should be a vector of 3 floats)
                if (accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    // Get the byte offset and size of the POSITION data
                    size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                    size_t byteLength = bufferView.byteLength;
                    
                    // Cast the raw buffer data to float and print the vertex positions
                    const float* positions = reinterpret_cast<const float*>(&buffer.data[byteOffset]);
                    vertices.resize(byteLength / sizeof(float) / 3);
                    for (size_t i = 0; i < byteLength / sizeof(float); i += 3) {
                        auto vertex = Vertex{};
                        //std::cout << "Vertex position: (" << positions[i] << ", " << positions[i + 1] << ", " << positions[i + 2] << ")\n";
                        vertex.pos = glm::vec3(positions[i], positions[i + 1], positions[i + 2]);
                        vertices.push_back(vertex);
                    }
                }
            } 
            if (primitive.indices > -1){
                const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;
                const uint8_t* indices = &buffer.data[byteOffset];

                if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE) {
                    // handle indices as unsigned bytes
                    const auto* indices_uint8 = reinterpret_cast<const uint8_t*>(indices);
                    std::cout << "[Indices Import] Unsigned byte - Currently not supported - Fix it Marvin! \n";
                    // ... use indices_uint8 ...
                } else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
                    // handle indices as unsigned shorts
                    size_t byteLength = bufferView.byteLength;
                    
                    const auto* indices_uint16 = reinterpret_cast<const uint16_t*>(indices);
                    vertices.resize(byteLength / sizeof(uint16_t));
                    for (size_t i = 0; i < byteLength / sizeof(uint16_t); ++i) {
                        std::cout << "Index: (" << indices_uint16[i] << ")\n";
                        
                    }
                } else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
                    // handle indices as unsigned ints
                    const auto* indices_uint32 = reinterpret_cast<const uint32_t*>(indices);
                    std::cout << "[Indices Import] Unsigned int - Currently not supported - Fix it Marvin! \n";
                    // ... use indices_uint32 ...
                }
            }
        }
    }
}
