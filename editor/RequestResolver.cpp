//
// Created by marvin on 15.11.2023.
//

#include "RequestResolver.h"
#include "Scene.h"
#include <iostream>
#include <boost/uuid/string_generator.hpp>
#include "EngineSystems.h"

namespace moonshine {
    void RequestResolver::resolve(boost::json::object jObj) {

        boost::json::string action = jObj["action"].get_string();
        if (std::equal(action.begin(), action.end(), "updateObject")) {
            boost::uuids::string_generator gen;
            boost::uuids::uuid id = gen(jObj["objectId"].get_string().c_str());
            Scene &scene = Scene::getCurrentScene();
            {
                scene.getLock();
                auto sceneObject = scene.get_by_id_unlocked(id);
                if (sceneObject != nullptr) {
                    sceneObject->getTransform()->deserialize(jObj);
                }
            }
        } else if (std::equal(action.begin(), action.end(), "updateScene")) {
            for (auto obj: jObj["sceneObjects"].as_array()) {
                resolve(obj.as_object());
            }
        } else if (std::equal(action.begin(), action.end(), "addObject")) {

            boost::json::string path = jObj["path"].get_string();
            boost::json::string name = jObj["name"].get_string();
            boost::uuids::string_generator gen;
            boost::uuids::uuid id = gen(jObj["objectId"].get_string().c_str());

            std::shared_ptr<WorkspaceManager> wkspaceMngr = EngineSystems::getInstance().get_workspace_manager();
            wkspaceMngr->import_object(wkspaceMngr->get_workspace_path() + "\\" + path.c_str(), name.c_str(), id);
        } else if (std::equal(action.begin(), action.end(), "lockUI")) {
            std::shared_ptr<UIManager> uiMngr = EngineSystems::getInstance().get_ui_manager();

            EngineSystems::getInstance().get_logger()->debug(LoggerType::Networking, boost::json::serialize(jObj));
            
            std::string label = jObj["label"].get_string().c_str();
            element_locker locker = boost::json::from_value(jObj["locker"]);
            locker.owner = other;
            uiMngr->register_field(label, locker, EngineSystems::getInstance().get_lobby_manager()->isHost());
        }
    }
} // moonshine