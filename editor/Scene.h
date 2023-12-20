//
// Created by marvin on 15.11.2023.
//

#ifndef MOONSHINE_SCENE_H
#define MOONSHINE_SCENE_H

#include <memory>
#include <vector>
#include "SceneObject.h"

namespace moonshine {

    class Scene {

    private:
        static Scene* current;

        std::shared_ptr<std::vector<std::shared_ptr<SceneObject>>> gameObjects;

        mutable std::mutex editGameObjectsMutex;
        
        void handle_remove(std::shared_ptr<SceneObject> object);

    public:
        static Scene &getCurrentScene();
        
        Scene(){
            current = this;
            gameObjects = std::make_shared<std::vector<std::shared_ptr<SceneObject>>>();
        }
        
        void add_object(std::shared_ptr<SceneObject> object);
        
        void remove_object(std::shared_ptr<SceneObject> object);

        std::shared_ptr<SceneObject> get_at(int index);

        std::shared_ptr<SceneObject> get_by_id(boost::uuids::uuid uuid);

        std::shared_ptr<SceneObject> get_by_id_unlocked(boost::uuids::uuid uuid);

        std::optional<size_t> get_object_index(const std::shared_ptr<SceneObject>& object) {
            std::unique_lock<std::mutex> lock(editGameObjectsMutex);
            auto it = std::find_if(gameObjects->begin(), gameObjects->end(),
                                   [&object](const std::shared_ptr<SceneObject>& obj) {
                                       return obj == object;
                                   });

            if (it != gameObjects->end()) {
                return std::distance(gameObjects->begin(), it);
            }
            return {}; // Return an empty std::optional if the object is not found
        }

        std::unique_lock<std::mutex> getLock() const {
            return std::unique_lock<std::mutex>(editGameObjectsMutex);
        }
        
        size_t get_size(){
            return gameObjects->size();
        }

        // Iterator methods
        auto begin() {
            return gameObjects->begin();
        }

        auto end() {
            return gameObjects->end();
        }

        // Const versions for iterating over const Scene objects
        auto begin() const {
            return gameObjects->cbegin();
        }

        auto end() const {
            return gameObjects->cend();
        }

    };
} // moonspace

#endif //MOONSHINE_SCENE_H
