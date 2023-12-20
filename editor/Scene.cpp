//
// Created by marvin on 15.11.2023.
//

#include <thread>
#include "Scene.h"
#include "EngineSystems.h"

namespace moonshine {

    Scene *Scene::current = nullptr;

    moonshine::Scene &moonshine::Scene::getCurrentScene() {
        if (current == nullptr) {
            throw std::runtime_error("Current scene is not set");
        }

        return *current;
    }

    void Scene::add_object(std::shared_ptr<SceneObject> object) {
        std::unique_lock<std::mutex> uniqueLock(editGameObjectsMutex);
        gameObjects->push_back(object);
    }

    void Scene::remove_object(std::shared_ptr<SceneObject> item) {

        if(item == nullptr) return;
        
        std::function<void()> deleteObject = [this, item] { handle_remove(item); };
        std::thread thread(deleteObject);
        thread.detach();
        
        EngineSystems::getInstance().get_lobby_manager()->replicateRemove(item->get_id_as_string());
    }

    std::shared_ptr<SceneObject> Scene::get_at(int index) {
        return gameObjects->at(index);
    }

    std::shared_ptr<SceneObject> Scene::get_by_id_unlocked(boost::uuids::uuid uuid) {
        auto it = std::find_if(gameObjects->begin(), gameObjects->end(),
                               [&uuid](const std::shared_ptr<SceneObject> &obj) {
                                   return obj->getId() == uuid;
                               });
        if (it != gameObjects->end()) {
            return *it;
        }

        return nullptr;
    }

    std::shared_ptr<SceneObject> Scene::get_by_id(boost::uuids::uuid uuid) {
        std::scoped_lock<std::mutex> uniqueLock(editGameObjectsMutex);
        return get_by_id_unlocked(uuid);
    }

    void Scene::handle_remove(std::shared_ptr<SceneObject> object) {
        {
            std::unique_lock<std::mutex> uniqueLock(editGameObjectsMutex);
            auto newEnd = std::remove(gameObjects->begin(), gameObjects->end(),
                                      object); // Move elements to keep to the front
            gameObjects->erase(newEnd, gameObjects->end()); // Erase the unwanted elements
        }
        // Wait, so the buffer isn't used in a vulkan command anymore. Should use fences before deleting the vkbuffer, but would require some bigger changes
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

} // moonspace

