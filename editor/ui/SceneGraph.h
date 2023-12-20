//
// Created by marvin on 30.09.2023.
//

#ifndef MOONSHINE_SCENEGRAPH_H
#define MOONSHINE_SCENEGRAPH_H

#include <memory>
#include "UIWindow.h"
#include "../SceneObject.h"
#include "../Scene.h"

namespace moonshine {

    class SceneGraph : public UIWindow {

    private:
        std::shared_ptr<SceneObject> m_selectedGameObject;
        std::shared_ptr<InputHandler> m_inputHandler;

        bool m_openPopup = false;
        std::shared_ptr<SceneObject> m_popupItem = nullptr;
        
        std::shared_ptr<SceneObject> m_deleteItem = nullptr;

    public:
        SceneGraph(std::shared_ptr<InputHandler>& inputHandler);

        void draw() override;

        std::shared_ptr<SceneObject> getSelected(){
            return m_selectedGameObject;
        }

    private:
        void showPopup(std::shared_ptr<SceneObject> &item);
        void handleDelete(Scene& scene, std::shared_ptr<SceneObject> item);
    };

} // moonshine

#endif //MOONSHINE_SCENEGRAPH_H
