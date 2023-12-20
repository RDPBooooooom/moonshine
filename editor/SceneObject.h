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

    private:
        std::string m_name;
        
        boost::uuids::uuid m_uniqueId;
        
        Transform& m_transform;

        std::vector<std::shared_ptr<Node>> nodes;
    public:

        explicit SceneObject(std::string &name, std::vector<std::shared_ptr<Node>> &data);
        explicit SceneObject(std::string &name, std::vector<std::shared_ptr<Node>> &data, boost::uuids::uuid uniqueId);
        ~SceneObject();

        void init(Device &device, std::shared_ptr<MaterialManager> &materialManager);

        const std::vector<std::shared_ptr<Node>> &get_nodes() const { return nodes; }

        Transform *getTransform() {
            return &m_transform;
        }

        std::basic_string<char> getName() {
            return m_name;
        }

        void setName(std::string name) {
            m_name = name;
        }

        boost::uuids::uuid getId() const { return m_uniqueId; }

        std::string get_id_as_string() const { return boost::uuids::to_string(m_uniqueId); }


    };

}
#endif //MOONSHINE_SCENEOBJECT_H
