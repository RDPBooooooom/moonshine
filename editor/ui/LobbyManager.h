//
// Created by marvin on 28.10.2023.
//

#ifndef MOONSHINE_LOBBYMANAGER_H
#define MOONSHINE_LOBBYMANAGER_H

#include "UIWindow.h"
#include "../../net/LobbyConnector.h"
#include "../InputHandler.h"

namespace moonshine {

    class LobbyManager : UIWindow {
        
    private:
        LobbyConnector connector;

        std::shared_ptr<InputHandler> m_inputHandler;

        bool isHosting = false;
        bool inSession = false;
        
        bool openHostPrompt = false;
        std::string lobbyName;

    public:


        explicit LobbyManager(std::shared_ptr<InputHandler>& inputHandler) : m_inputHandler(inputHandler){
            
        };

        void draw() override;

        void showPopup();
    };

} // moonshine

#endif //MOONSHINE_LOBBYMANAGER_H
