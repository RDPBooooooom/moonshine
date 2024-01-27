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

        boost::asio::io_context m_io_service;
        tcp::resolver m_resolver;
        TcpConnection::pointer m_connection;
        bool m_isConnected = false;
        SafeQueue<boost::json::value> m_messageQueue;

        bool m_thread_stop = false;
        std::thread m_thread;

        std::thread m_io_context_thread;

        std::mutex m_host_mutex;
        std::vector<std::shared_ptr<Host>> m_current_hosts;
        int m_item_current_idx = 0;

    public:

        LobbyConnector() : m_io_service(), m_resolver(m_io_service) {
            m_connection =
                    TcpConnection::create(m_io_service, m_messageQueue);
        }

        ~LobbyConnector() {
            if (m_thread.joinable()) {
                m_thread.join();
            }
            if (m_io_context_thread.joinable()) {
                m_io_context_thread.join();
            }
        }

        void handle_requests();

        void draw_hosts();

        bool is_connected() const {
            return m_isConnected;
        }

        void try_connect();

        void register_as_host(const std::string& name, int port);
        
        void unregister_as_host();

        void receive_hosts();

        void disconnect();

        std::shared_ptr<LobbyConnector::Host> get_selected_host();
        
    private:
        void connect();
    };

} // moonshine

#endif //MOONSHINE_LOBBYCONNECTOR_H
