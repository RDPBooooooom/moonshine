//
// Created by marvin on 30.09.2023.
//

#ifndef MOONSHINE_SCENEGRAPH_H
#define MOONSHINE_SCENEGRAPH_H

#include <memory>
#include "UIWindow.h"
#include "../SceneObject.h"
#include "../Scene.h"
#include "../Camera.h"

namespace moonshine {

    class SceneGraph : public UIWindow {

    private:
        std::shared_ptr<SceneObject> m_selected_game_object;
        std::shared_ptr<InputHandler> m_input_handler;

        bool m_open_popup = false;
        std::shared_ptr<SceneObject> m_popup_item = nullptr;

        std::shared_ptr<SceneObject> m_delete_item = nullptr;

        Camera &m_camera;

        int m_focus_selected_input_handle = -1;

    public:
        SceneGraph(std::shared_ptr<InputHandler> &inputHandler, Camera &camera);

        ~SceneGraph() override;

        void draw() override;

        void focus_selected(bool isReleased);

        std::shared_ptr<SceneObject> getSelected() {
            return m_selected_game_object;
        }


    private:
        void show_popup(std::shared_ptr<SceneObject> &item);

        void handle_delete(Scene &scene, std::shared_ptr<SceneObject> item);

    };

} // moonshine

#endif //MOONSHINE_SCENEGRAPH_H
