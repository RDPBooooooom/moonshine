﻿//
// Created by marvin on 15.11.2023.
//

#include "Scene.h"

namespace moonshine {

    Scene* Scene::current = nullptr;
    
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

    void Scene::remove_object(std::shared_ptr<SceneObject> object) {

    }

    std::shared_ptr<SceneObject> Scene::get_at(int index) {
        return gameObjects->at(index);
    }

    std::shared_ptr<SceneObject> Scene::get_by_id_unlocked(boost::uuids::uuid uuid) {
      auto it = std::find_if(gameObjects->begin(), gameObjects->end(),
                             [&uuid](const std::shared_ptr<SceneObject>& obj) {
                                 return obj->getId() == uuid;
                             });
        if(it != gameObjects->end()){
            return *it;
        }

        return nullptr;
    }

    std::shared_ptr<SceneObject> Scene::get_by_id(boost::uuids::uuid uuid) {
        std::scoped_lock<std::mutex> uniqueLock(editGameObjectsMutex);
        return get_by_id_unlocked(uuid);
    }

} // moonspace
