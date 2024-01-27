//
// Created by marvin on 28.10.2023.
//

#ifndef MOONSHINE_LOBBYMANAGER_H
#define MOONSHINE_LOBBYMANAGER_H

#include <winsock2.h>
#include "../../net/LobbyConnector.h"
#include "UIWindow.h"
#include "../InputHandler.h"
#include "../../net/Server.h"
#include "../../net/Client.h"
#include "net/UIManager.h"

namespace moonshine {

    class LobbyManager : UIWindow {

    private:
        LobbyConnector m_connector;

        std::shared_ptr<InputHandler> m_inputHandler;
        std::shared_ptr<net::Server> m_server;
        std::shared_ptr<net::Client> m_client;

        bool m_is_hosting = false;
        bool m_in_session = false;

        bool m_open_host_prompt = false;
        std::string m_lobby_name;

        float m_time_since_last_replication = 0.0f;

    public:


        explicit LobbyManager(std::shared_ptr<InputHandler> &inputHandler) : m_inputHandler(inputHandler) {
            m_client = std::make_shared<net::Client>();
        };

        void draw() override;

        void show_popup();

        void replicate();

        void replicate(std::shared_ptr<SceneObject> &object);

        void replicate_add(std::string path, std::string name, std::string uuid, Transform transform);

        void replicate_remove(std::string uuid);

        void replicate_ui(std::string &label, element_locker &locker);

        void replicate_rename(std::string uuid, std::string name);

        bool is_host() {
            return m_is_hosting;
        }

    private:
        void start_hosting();

    };

} // moonshine

#endif //MOONSHINE_LOBBYMANAGER_H
