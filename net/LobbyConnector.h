//
// Created by marvin on 28.10.2023.
//

#ifndef MOONSHINE_LOBBYCONNECTOR_H
#define MOONSHINE_LOBBYCONNECTOR_H

#include <winsock2.h>
#include "TcpConnection.h"
#include "imgui.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace moonshine {

    class LobbyConnector {

    public:
        struct Host {
            int64_t id;
            std::string name;
            std::string ipv4;
            int64_t port;
        };

    private:

        boost::asio::io_context io_service;
        tcp::resolver resolver;
        TcpConnection::pointer connection;
        bool m_isConnected = false;
        SafeQueue<boost::json::value> m_messageQueue;

        bool threadStop = false;
        std::thread thread;

        std::thread ioContextThread;

        std::mutex hostMutex;
        std::unique_ptr<std::vector<Host>> currentHosts;
        int item_current_idx = 0;

    public:

        LobbyConnector() : io_service(), resolver(io_service) {
            connection =
                    TcpConnection::create(io_service, m_messageQueue);

            currentHosts = std::make_unique<std::vector<Host>>();
        }

        ~LobbyConnector() {
            if (thread.joinable()) {
                thread.join();
            }
            if (ioContextThread.joinable()) {
                ioContextThread.join();
            }
        }

        void handleRequests();

        void drawHosts();

        bool isConnected() const {
            return m_isConnected;
        }

        void connect();

        void registerAsHost(const std::string& name, int port);

        void receiveHosts();

        void disconnect();

        LobbyConnector::Host getSelectedHost();
    };

} // moonshine

#endif //MOONSHINE_LOBBYCONNECTOR_H
