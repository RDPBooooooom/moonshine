//
// Created by marvin on 28.10.2023.
//

#ifndef MOONSHINE_LOBBYMANAGER_H
#define MOONSHINE_LOBBYMANAGER_H

#include "UIWindow.h"
#include "../../net/LobbyConnector.h"
#include "../InputHandler.h"
#include "../../net/Server.h"
#include "../../net/Client.h"

namespace moonshine {

    class LobbyManager : UIWindow {

    private:
        LobbyConnector connector;

        std::shared_ptr<InputHandler> m_inputHandler;
        std::shared_ptr<net::Server> m_server;
        std::shared_ptr<net::Client> m_client;

        bool isHosting = false;
        bool inSession = false;

        bool openHostPrompt = false;
        std::string lobbyName;

    public:


        explicit LobbyManager(std::shared_ptr<InputHandler> &inputHandler) : m_inputHandler(inputHandler) {
            m_client = std::make_shared<net::Client>();
        };

        void draw() override;

        void showPopup();

        void replicate();

        void replicate(std::shared_ptr<SceneObject> &object);

    private:
        void start_hosting();

    };

} // moonshine

#endif //MOONSHINE_LOBBYMANAGER_H
