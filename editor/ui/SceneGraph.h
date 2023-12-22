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
        std::shared_ptr<SceneObject> m_selectedGameObject;
        std::shared_ptr<InputHandler> m_inputHandler;

        bool m_openPopup = false;
        std::shared_ptr<SceneObject> m_popupItem = nullptr;
        
        std::shared_ptr<SceneObject> m_deleteItem = nullptr;
        
        Camera &m_camera;
        
        int focus_selected_input_handle = -1;

    public:
        SceneGraph(std::shared_ptr<InputHandler>& inputHandler, Camera &camera);
        ~SceneGraph() override;

        void draw() override;

        void focus_selected(bool isReleased);
        std::shared_ptr<SceneObject> getSelected(){
            return m_selectedGameObject;
        }
        

    private:
        void showPopup(std::shared_ptr<SceneObject> &item);
        void handleDelete(Scene& scene, std::shared_ptr<SceneObject> item);

    };

} // moonshine

#endif //MOONSHINE_SCENEGRAPH_H
