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
        static Scene* s_current;

        std::shared_ptr<std::vector<std::shared_ptr<SceneObject>>> m_game_objects;

        mutable std::mutex m_edit_game_objects_mutex;
        
        void handle_remove(std::shared_ptr<SceneObject> object);

    public:
        static Scene &get_current_scene();
        
        Scene(){
            s_current = this;
            m_game_objects = std::make_shared<std::vector<std::shared_ptr<SceneObject>>>();
        }
        
        void add_object(std::shared_ptr<SceneObject> object);
        
        void remove_object(std::shared_ptr<SceneObject> object);

        std::shared_ptr<SceneObject> get_at(int index);

        std::shared_ptr<SceneObject> get_by_id(boost::uuids::uuid uuid);

        std::shared_ptr<SceneObject> get_by_id_unlocked(boost::uuids::uuid uuid);

        std::optional<size_t> get_object_index(const std::shared_ptr<SceneObject>& object) {
            std::unique_lock<std::mutex> lock(m_edit_game_objects_mutex);
            auto it = std::find_if(m_game_objects->begin(), m_game_objects->end(),
                                   [&object](const std::shared_ptr<SceneObject>& obj) {
                                       return obj == object;
                                   });

            if (it != m_game_objects->end()) {
                return std::distance(m_game_objects->begin(), it);
            }
            return {}; // Return an empty std::optional if the object is not found
        }

        std::unique_lock<std::mutex> get_lock() const {
            return std::unique_lock<std::mutex>(m_edit_game_objects_mutex);
        }
        
        size_t get_size(){
            return m_game_objects->size();
        }

        // Iterator methods
        auto begin() {
            return m_game_objects->begin();
        }

        auto end() {
            return m_game_objects->end();
        }

        // Const versions for iterating over const Scene objects
        auto begin() const {
            return m_game_objects->cbegin();
        }

        auto end() const {
            return m_game_objects->cend();
        }

        boost::json::object serialize();

    };
} // moonspace

#endif //MOONSHINE_SCENE_H
