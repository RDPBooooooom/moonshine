//
// Created by marvin on 16.11.2023.
//

#ifndef MOONSHINE_WORKSPACEMANAGER_H
#define MOONSHINE_WORKSPACEMANAGER_H

#include <string>
#include <memory>
#include "../InputHandler.h"

namespace moonshine {

    class WorkspaceManager {

    private:
        std::shared_ptr<InputHandler> m_inputHandler;

        std::string m_workspacePath;
        bool m_workspaceModalActive = false;


        void drawInitModal();

    public:

        explicit WorkspaceManager(std::shared_ptr<InputHandler> &inputHandler) : m_inputHandler(inputHandler) {
        }
        
        void draw();

    };

} // moonshine

#endif //MOONSHINE_WORKSPACEMANAGER_H
