//
// Created by marvin on 22.06.2023.
//
#include <iostream>
#include "SceneObject.h"
#include "Node.h"
#include <boost/uuid/uuid_generators.hpp>
#include "EngineSystems.h"

namespace moonshine {

    void SceneObject::init(Device &device, std::shared_ptr<MaterialManager> &material_manager) {

        for (std::shared_ptr<Node> node: m_nodes) {
            for (auto &mesh: node->get_sub_meshes_editable()) {
                mesh.m_vertexBuffer = std::make_unique<GpuBuffer<Vertex>>(mesh.m_vertices, device,
                                                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

                auto logger = EngineSystems::get_instance().get_logger();
                logger->debug(LoggerType::Rendering, "Finished vertex buffer => creating index buffer next");

                mesh.m_indexBuffer = std::make_unique<GpuBuffer<uint16_t>>(mesh.m_indices, device,
                                                                           VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

                logger->debug(LoggerType::Rendering,  "finished index buffer");

                mesh.m_materialIdx = material_manager->create_material(m_name, mesh.m_texName, mesh.m_path);
            }
        }
    }

    SceneObject::SceneObject(std::string &name, std::vector<std::shared_ptr<Node>> &child_nodes) : m_transform{
            child_nodes[0]->get_transform()}, m_nodes{child_nodes}, m_unique_id(boost::uuids::random_generator()()) {
        m_name = name;

    }

    SceneObject::SceneObject(std::string &name, std::vector<std::shared_ptr<Node>> &child_nodes,
                             boost::uuids::uuid unique_id)
            : m_transform{
            child_nodes[0]->get_transform()}, m_nodes{child_nodes},
              m_unique_id{unique_id} {
        m_name = name;
    }

    SceneObject::~SceneObject() {
        EngineSystems::get_instance().get_logger()->debug(LoggerType::Editor, "Destroyed {}", to_string(m_unique_id));
    }
}