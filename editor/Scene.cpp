//
// Created by marvin on 15.11.2023.
//

#include <thread>
#include "Scene.h"
#include "EngineSystems.h"

namespace moonshine {

    Scene *Scene::s_current = nullptr;

    moonshine::Scene &moonshine::Scene::get_current_scene() {
        if (s_current == nullptr) {
            throw std::runtime_error("Current scene is not set");
        }

        return *s_current;
    }

    void Scene::add_object(std::shared_ptr<SceneObject> object) {
        std::unique_lock<std::mutex> unique_lock(m_edit_game_objects_mutex);
        m_game_objects->push_back(object);
    }

    void Scene::remove_object(std::shared_ptr<SceneObject> item) {

        if (item == nullptr) return;

        std::function<void()> delete_object = [this, item] { handle_remove(item); };
        std::thread thread(delete_object);
        thread.detach();

        EngineSystems::get_instance().get_lobby_manager()->replicate_remove(item->get_id_as_string());
    }

    std::shared_ptr<SceneObject> Scene::get_at(int index) {
        return m_game_objects->at(index);
    }

    std::shared_ptr<SceneObject> Scene::get_by_id_unlocked(boost::uuids::uuid uuid) {
        auto it = std::find_if(m_game_objects->begin(), m_game_objects->end(),
                               [&uuid](const std::shared_ptr<SceneObject> &obj) {
                                   return obj->get_id() == uuid;
                               });
        if (it != m_game_objects->end()) {
            return *it;
        }

        return nullptr;
    }

    std::shared_ptr<SceneObject> Scene::get_by_id(boost::uuids::uuid uuid) {
        std::scoped_lock<std::mutex> scoped_lock(m_edit_game_objects_mutex);
        return get_by_id_unlocked(uuid);
    }

    void Scene::handle_remove(std::shared_ptr<SceneObject> object) {
        {
            std::unique_lock<std::mutex> unique_lock(m_edit_game_objects_mutex);
            auto new_end = std::remove(m_game_objects->begin(), m_game_objects->end(),
                                       object); // Move elements to keep to the front
            m_game_objects->erase(new_end, m_game_objects->end()); // Erase the unwanted elements
        }
        // Wait, so the buffer isn't used in a vulkan command anymore. Should use fences before deleting the vkbuffer, but would require some bigger changes
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    boost::json::object Scene::serialize() {

        boost::json::object scene;
        boost::json::array scene_objects;
        
        for (const auto& item: *m_game_objects) {
            boost::json::object game_obj;
            game_obj["objectId"] = item->get_id_as_string();
            game_obj["name"] = item->get_name();

            game_obj["transform"] = item->get_transform()->serialize();
            
            SceneObject::object_meta_data meta_data = item->get_meta_data();
            game_obj["path"] = meta_data.path;
            game_obj["filename"] = meta_data.filename;
            
            scene_objects.push_back(game_obj);
        }
        scene["objects"] = scene_objects;

        return scene;
    }

} // moonspace

