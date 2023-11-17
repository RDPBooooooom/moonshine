//
// Created by marvin on 17.11.2023.
//

#ifndef MOONSHINE_MESH_H
#define MOONSHINE_MESH_H

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include "../../utils/VkUtils.h"
#include "../../graphics/GpuBuffer.h"

namespace moonshine {

    struct Mesh {
        uint16_t m_materialIdx;
        std::vector<Vertex> m_vertices;
        std::vector<uint16_t> m_indices;

        std::string m_matName;
        std::string m_texName;
        std::string m_path;

        std::unique_ptr<GpuBuffer<Vertex>> m_vertexBuffer;
        std::unique_ptr<GpuBuffer<uint16_t>> m_indexBuffer;
    };

} // moonshine

#endif //MOONSHINE_MESH_H
