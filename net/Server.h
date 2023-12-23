//
// Created by marvin on 04.11.2023.
//

#ifndef MOONSHINE_SERVER_H
#define MOONSHINE_SERVER_H

#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "TcpConnection.h"
#include "../editor/RequestResolver.h"
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include "../editor/Scene.h"
#include "../editor/ui/net/UIManager.h"

using boost::asio::ip::tcp;

namespace moonshine::net {

    class Server {

    private:
        boost::asio::io_context m_ioContext;
        tcp::acceptor m_acceptor;

        SafeQueue<boost::json::value> m_messageQueue;

        std::mutex m_clients;
        std::vector<TcpConnection::pointer> m_connectedClients;

        std::thread m_ioThread;
        std::thread m_receiveThread;

        RequestResolver resolver;

        struct UPNPDev *m_deviceList;
        struct UPNPUrls m_urls;
        struct IGDdatas m_data;

        bool wasUpnpFreed = false;
        bool threadStop = false;

    public:

        explicit Server() :
                m_ioContext(),
                m_acceptor(m_ioContext, tcp::endpoint(tcp::v4(), 0)),
                m_messageQueue(),
                m_connectedClients() {

            start_accept();
            m_receiveThread = std::thread([this] { handleRequests(); });
            m_ioThread = std::thread([ObjectPtr = &m_ioContext] { return ObjectPtr->run(); });

            do_upnp();

        }

        ~Server() {
            free_upnp();
            if (m_ioThread.joinable()) {
                m_ioThread.join();
            }
        }

        int get_port() {
            return m_acceptor.local_endpoint().port();
        }

        void stop();

        void broadcast(moonshine::Scene &scene);

        void broadcast(std::string &path, std::string &name, std::string &uuid, Transform transform);

        void broadcast(std::string &label, element_locker locker);

        void broadcast(std::string &uuid);

        void broadcast(std::string &uuid, std::string &name);

    private:

        void start_accept();

        void on_accept(TcpConnection::pointer &new_connection,
                       const boost::system::error_code &error);

        void do_upnp();

        void free_upnp();

        void handleRequests();

    };

} // moonshine
// net

#endif //MOONSHINE_SERVER_H
