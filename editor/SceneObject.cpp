//
// Created by marvin on 22.06.2023.
//
#include <iostream>
#include "SceneObject.h"
#include "Node.h"
#include <boost/uuid/uuid_generators.hpp>

namespace moonshine {

    void SceneObject::init(Device &device, std::shared_ptr<MaterialManager> &materialManager) {

        for (std::shared_ptr<Node> node: nodes) {
            for (auto &mesh: node->get_sub_meshes_editable()) {
                mesh.m_vertexBuffer = std::make_unique<GpuBuffer<Vertex>>(mesh.m_vertices, device,
                                                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

                std::cout << "Finished vertex buffer => creating index buffer next \n";

                mesh.m_indexBuffer = std::make_unique<GpuBuffer<uint16_t>>(mesh.m_indices, device,
                                                                           VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

                std::cout << "finished index buffer \n";

                mesh.m_materialIdx = materialManager->createMaterial(m_name, mesh.m_texName, mesh.m_path);
            }
        }
    }

    SceneObject::SceneObject(std::string &name, std::vector<std::shared_ptr<Node>> &childNodes) : m_transform{
            childNodes[0]->get_transform()}, nodes{childNodes}, m_uniqueId(boost::uuids::random_generator()()) {
        m_name = name;

    }

    SceneObject::SceneObject(std::string &name, std::vector<std::shared_ptr<Node>> &childNodes,
                             boost::uuids::uuid uniqueId)
            : m_transform{
            childNodes[0]->get_transform()}, nodes{childNodes},
              m_uniqueId{uniqueId} {
        m_name = name;
    }
}