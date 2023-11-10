//
// Created by marvin on 04.11.2023.
//

#ifndef MOONSHINE_SERVER_H
#define MOONSHINE_SERVER_H

#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "TcpConnection.h"

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

    public:
        explicit Server() :
                m_ioContext(),
                m_acceptor(m_ioContext, tcp::endpoint(tcp::v4(), 0)),
                m_messageQueue(),
                m_connectedClients() {

            start_accept();
            m_ioThread = std::thread([ObjectPtr = &m_ioContext] { return ObjectPtr->run(); });
        }

        ~Server() {
            if (m_ioThread.joinable()) {
                m_ioThread.join();
            }
        }

        int get_port() {
            return m_acceptor.local_endpoint().port();
        }

        void stop();

    private:

        void start_accept();

        void on_accept(TcpConnection::pointer new_connection,
                       const boost::system::error_code &error);

    };

} // moonshine
// net

#endif //MOONSHINE_SERVER_H
