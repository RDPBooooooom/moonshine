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
            Scene &scene = Scene::get_current_scene();
            {
                scene.get_lock();
                auto scene_object = scene.get_by_id_unlocked(id);
                if (scene_object != nullptr) {
                    scene_object->get_transform()->deserialize(jObj);
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
            Transform transform = {};
            transform.deserialize(jObj["transform"].as_object());

            std::shared_ptr<WorkspaceManager> wkspace_mngr = EngineSystems::get_instance().get_workspace_manager();
            wkspace_mngr->import_object(wkspace_mngr->get_workspace_path() + "\\" + path.c_str(), name.c_str(), id, transform);
        } else if(std::equal(action.begin(), action.end(), "removeObject")){
            boost::uuids::string_generator gen;
            boost::uuids::uuid id = gen(jObj["objectId"].get_string().c_str());

            Scene &scene = Scene::get_current_scene();
            scene.remove_object(scene.get_by_id(id));
        } else if (std::equal(action.begin(), action.end(), "lockUI")) {
            std::shared_ptr<UIManager> ui_mngr = EngineSystems::get_instance().get_ui_manager();

            EngineSystems::get_instance().get_logger()->debug(LoggerType::Networking, boost::json::serialize(jObj));
            
            std::string label = jObj["label"].get_string().c_str();
            element_locker locker = boost::json::from_value(jObj["locker"]);
            locker.owner = other;
            ui_mngr->register_field(label, locker, EngineSystems::get_instance().get_lobby_manager()->is_host());
        } else if(std::equal(action.begin(), action.end(), "renameObject")){
            boost::uuids::string_generator gen;
            boost::uuids::uuid id = gen(jObj["objectId"].get_string().c_str());
            Scene &scene = Scene::get_current_scene();
            scene.get_by_id(id)->set_name(jObj["name"].as_string().c_str());

            auto lobby_manager = EngineSystems::get_instance().get_lobby_manager();
            if(lobby_manager->is_host()){
                lobby_manager->replicate_rename(jObj["objectId"].as_string().c_str(), jObj["name"].as_string().c_str());
            }
        }
    }
} // moonshine