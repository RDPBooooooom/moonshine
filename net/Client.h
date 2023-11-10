//
// Created by marvin on 04.11.2023.
//

#ifndef MOONSHINE_CLIENT_H
#define MOONSHINE_CLIENT_H

#include "boost/asio/ip/tcp.hpp"
#include "TcpConnection.h"

using boost::asio::ip::tcp;

namespace moonshine::net {

    class Client {

    private:

        std::thread m_ioContextThread;
        boost::asio::io_context m_ioContext;
        tcp::resolver m_resolver;

        TcpConnection::pointer m_connection;

        SafeQueue<boost::json::value> m_messageQueue;

    public:
        Client() : m_ioContext(), m_resolver(m_ioContext), m_messageQueue() {

            m_connection =
                    TcpConnection::create(m_ioContext, m_messageQueue);
        }

        void connect(std::string ipv4, int64_t port);

        void disconnect();

    };

} // moonshine
// net

#endif //MOONSHINE_CLIENT_H
