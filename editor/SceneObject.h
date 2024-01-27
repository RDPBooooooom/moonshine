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
#include "Mesh.h"
#include "Node.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace moonshine {

    class SceneObject {

    public:
        struct object_meta_data {
            std::string filename = "";
            std::string path = "";
        };

    private:
        std::string m_name;

        boost::uuids::uuid m_unique_id;

        Transform &m_transform;

        std::vector<std::shared_ptr<Node>> m_nodes;

        object_meta_data m_meta_data;
    public:

        explicit SceneObject(std::string &name, std::vector<std::shared_ptr<Node>> &child_nodes);

        explicit SceneObject(std::string &name, std::vector<std::shared_ptr<Node>> &child_nodes, boost::uuids::uuid unique_id);

        ~SceneObject();

        void init(Device &device, std::shared_ptr<MaterialManager> &material_manager);

        const std::vector<std::shared_ptr<Node>> &get_nodes() const { return m_nodes; }

        Transform *get_transform() {
            return &m_transform;
        }
        
        void set_transform(Transform &transform){
            m_transform = transform;
        }

        std::basic_string<char> get_name() {
            return m_name;
        }

        void set_name(std::string name) {
            m_name = name;
        }

        object_meta_data &get_meta_data() {
            return m_meta_data;
        }
        
        void set_meta_data(object_meta_data &meta_data){
            this->m_meta_data = meta_data;
        }

        boost::uuids::uuid get_id() const { return m_unique_id; }

        std::string get_id_as_string() const { return boost::uuids::to_string(m_unique_id); }

        std::string as_string() { return get_name() + "(" + get_id_as_string() + ")"; };
    };

}
#endif //MOONSHINE_SCENEOBJECT_H
