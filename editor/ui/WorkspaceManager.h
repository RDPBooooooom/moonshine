//
// Created by marvin on 16.11.2023.
//

#ifndef MOONSHINE_WORKSPACEMANAGER_H
#define MOONSHINE_WORKSPACEMANAGER_H

#include <string>
#include <memory>
#include "../InputHandler.h"
#include "../../graphics/Device.h"
#include "../../graphics/MaterialManager.h"
#include "../Transform.h"
#include <boost/uuid/uuid.hpp>
#include <boost/json/array.hpp>

namespace moonshine {

    class WorkspaceManager {

    private:
        Device &m_device;

        std::shared_ptr<InputHandler> m_inputHandler;
        std::shared_ptr<MaterialManager> m_materialManager;

        std::string m_workspacePath;

        struct import_data {
            std::string path = "";
            std::string file = "";
            std::string name = "";
            boost::uuids::uuid uuid;
            
            bool overwrite_transform = false;
            Transform transform = {};
        };

    public:
        const std::string &get_workspace_path() const;

    private:
        bool m_workspaceModalActive = false;


        void drawInitModal();

        void import_object_gltf(import_data data);

        void load_workspace_scene();

        void handle_load_scene(const boost::json::array &objects);

        void import_object(import_data &data);

    public:

        explicit WorkspaceManager(Device &device, std::shared_ptr<MaterialManager> &materialManager,
                                  std::shared_ptr<InputHandler> &inputHandler) : m_inputHandler{inputHandler},
                                                                                 m_device{device},
                                                                                 m_materialManager{materialManager} {
            
        }

        void draw();

        void import_object(std::string path, std::string file);

        void import_object(std::string path, std::string file, boost::uuids::uuid uuid);

        void save_scene();

    };

} // moonshine

#endif //MOONSHINE_WORKSPACEMANAGER_H
