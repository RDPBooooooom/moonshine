﻿//
// Created by marvin on 04.11.2023.
//

#ifndef MOONSHINE_CLIENT_H
#define MOONSHINE_CLIENT_H

#include "boost/asio/ip/tcp.hpp"
#include "TcpConnection.h"
#include "../editor/RequestResolver.h"
#include "../editor/SceneObject.h"
#include "../editor/ui/net/UIManager.h"

using boost::asio::ip::tcp;

namespace moonshine::net {

    class Client {

    private:

        std::thread m_ioContextThread;
        boost::asio::io_context m_ioContext;
        tcp::resolver m_resolver;

        TcpConnection::pointer m_connection;

        SafeQueue<boost::json::value> m_messageQueue;

        RequestResolver m_request_resolver;
        std::thread m_receiveThread;
        bool m_thread_stop = false;

    public:
        Client() : m_ioContext(), m_resolver(m_ioContext), m_messageQueue() {

            m_connection =
                    TcpConnection::create(m_ioContext, m_messageQueue);
        }

        void connect(std::string ipv4, int64_t port);

        void disconnect();

        void send(std::shared_ptr<SceneObject> &object);

        void send(std::string &path, std::string &name, std::string &uuid, Transform transform);

        void send(std::string &label, element_locker locker);

        void send(std::string &uuid);

        void send(std::string &uuid, std::string &name);

    private:
        void handle_requests();

    };

} // moonshine
// net

#endif //MOONSHINE_CLIENT_H
