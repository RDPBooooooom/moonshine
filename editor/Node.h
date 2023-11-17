//
// Created by marvin on 17.11.2023.
//

#ifndef MOONSHINE_NODE_H
#define MOONSHINE_NODE_H

#include <vector>
#include "Mesh.h"
#include "Transform.h"

namespace moonshine {

    struct GltfData {
        std::vector<Vertex> m_vertices;
        std::vector<uint16_t> m_indices;

        std::string m_path;
        std::string m_name;
        std::string m_matName;
        std::string m_texName;
    };
    
    class Node {

    private:
        std::vector<Mesh> m_meshes;
        std::shared_ptr<Node> m_parent;
        Transform m_transform = {};

    public:
        Node(std::shared_ptr<Node> &parent, Transform &transform) : m_parent{parent}, m_transform{transform} {
        }

        void set_gltf_data(std::vector<GltfData> &dataList) {
            for (const auto &data: dataList) {
                Mesh mesh = {};
                mesh.m_texName = data.m_texName;
                mesh.m_matName = data.m_matName;
                mesh.m_indices = data.m_indices;
                mesh.m_vertices = data.m_vertices;
                mesh.m_path = data.m_path;

                m_meshes.push_back(std::move(mesh));
            }
        }

        const std::vector<Mesh> &get_sub_meshes() const { return m_meshes; }

        std::vector<Mesh> &get_sub_meshes_editable() { return m_meshes; }
        
        Transform &get_transform() {
            return m_transform;
        }
        
        bool has_parent(){
            return m_parent != nullptr;
        }

        glm::mat4 get_submesh_matrix() {
            if (has_parent()){
                return m_parent->get_submesh_matrix() * m_transform.getMatrix();
            }
            return m_transform.getMatrix();
        }
    };

} // moonshine

#endif //MOONSHINE_NODE_H
