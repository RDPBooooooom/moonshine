//
// Created by marvin on 22.06.2023.
//

#ifndef MOONSHINE_SCENEOBJECT_H
#define MOONSHINE_SCENEOBJECT_H

#include <memory>
#include <utility>
#include "Transform.h"
#include "../utils/VkUtils.h"
#include "../graphics/GpuBuffer.h"
#include "../graphics/MaterialManager.h"

namespace moonshine {
    struct GltfData{
        std::vector<Vertex> m_vertices;
        std::vector<uint16_t> m_indices;

        std::string m_path;
        std::string m_name;
        std::string m_matName;
        std::string m_texName;
    };
    
    class SceneObject {

    private:
        std::string m_name;
        Transform m_transform;
        uint16_t m_materialIdx;
        std::vector<Vertex> m_vertices;
        std::vector<uint16_t> m_indices;

        std::string m_matName;
        std::string m_texName;
        std::string m_path;

        std::unique_ptr<GpuBuffer<Vertex>> m_vertexBuffer;
        std::unique_ptr<GpuBuffer<uint16_t>> m_indexBuffer;
    public:
        
        SceneObject(std::string filepath, std::string filename);

        explicit SceneObject(GltfData &data);

        void init(Device &device, std::shared_ptr<MaterialManager> &materialManager);

        VkBuffer getVertBuffer() {
            return m_vertexBuffer->getBuffer();
        }

        size_t getIndexSize() {
            return m_indices.size();
        }

        VkBuffer getIndexBuffer() {
            return m_indexBuffer->getBuffer();
        }

        Transform *getTransform() {
            return &m_transform;
        }

        std::basic_string<char> getName() {
            return m_name;
        }

        void setName(std::string name) {
            m_name = name;
        }

        uint16_t getMaterialIdx() const {
            return m_materialIdx;
        }

        void setMaterialIdx(uint16_t newIdx) {
            m_materialIdx = newIdx;
        }

    };

}
#endif //MOONSHINE_SCENEOBJECT_H
