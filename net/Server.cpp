//
// Created by marvin on 04.11.2023.
//

#include "Server.h"


namespace moonshine::net {

    void Server::start_accept() {
        TcpConnection::pointer new_connection =
                TcpConnection::create(m_ioContext, m_messageQueue);

        std::cout << "[Server] Waiting..." << std::endl;;

        m_acceptor.async_accept(new_connection->socket(),
                                bind(&Server::on_accept, this, new_connection,
                                     boost::asio::placeholders::error));
    }

    void Server::on_accept(TcpConnection::pointer new_connection, const error_code &error) {
        if (!error) {
            std::cout << "[Server] New Connection by " << new_connection->socket().remote_endpoint().address() << ":"
                      << new_connection->socket().remote_endpoint().port() << std::endl;
            
            new_connection->start();
            
            m_clients.lock();
            m_connectedClients.push_back(new_connection);
            m_clients.unlock();

        }

        start_accept();
    }

    void Server::stop() {
        m_clients.lock();
        // Close the connection
        for (const auto &item: m_connectedClients) {
            item->socket().close();
        }
        m_clients.unlock();

        // Stop the IO context to allow the thread to finish
        m_ioContext.stop();

        // Join the thread if it's running
        if (m_ioThread.joinable()) {
            m_ioThread.join();
        }
    }

} // moonshine
// net