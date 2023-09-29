﻿//
// Created by marvin on 22.06.2023.
//

#ifndef MOONSHINE_SCENEOBJECT_H
#define MOONSHINE_SCENEOBJECT_H

#include <memory>
#include "../external/tinygltf/tiny_gltf.h"
#include "Transform.h"
#include "../utils/VkUtils.h"
#include "../graphics/GpuBuffer.h"

namespace moonshine {
    class SceneObject {

    private:
        tinygltf::Model m_model;
        Transform m_transform;
        std::vector<Vertex> m_vertices;
        std::vector<uint16_t> m_indices;

    public:
        std::unique_ptr<GpuBuffer<Vertex>> m_vertexBuffer;
        std::unique_ptr<GpuBuffer<uint16_t>> m_indexBuffer;
        explicit SceneObject(const char *filepath, Device &device);

        VkBuffer getVertBuffer(){
            return m_vertexBuffer->getBuffer();
        }

        size_t getIndexSize(){
            return m_indices.size();
        }
        
        VkBuffer const getIndexBuffer(){
            return m_indexBuffer->getBuffer();
        }
        
        Transform* getTransform(){
            return &m_transform;
        }
    };

}
#endif //MOONSHINE_SCENEOBJECT_H