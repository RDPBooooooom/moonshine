//
// Created by marvin on 04.11.2023.
//

#include "Client.h"


namespace moonshine::net {

    void Client::connect(std::string ipv4, int64_t port) {
        m_connection->start(m_resolver.resolve(ipv4, std::to_string(port)));

        m_connection.get()->async_receive_json();
        if (m_ioContext.stopped()) {
            m_ioContext.restart();
        }
        m_ioContextThread = std::thread([ObjectPtr = &m_ioContext] { return ObjectPtr->run(); });
    }

    void Client::disconnect() {
        if (m_connection->socket().is_open()) {
            try {
                // Close the connection
                m_connection->socket().close();

                // Stop the IO context to allow the thread to finish
                m_ioContext.stop();

                // Join the thread if it's running
                if (m_ioContextThread.joinable()) {
                    m_ioContextThread.join();
                }

                std::cout << "[Client] Disconnected" << std::endl;
                m_messageQueue.notifyToStop();

                /*if (thread.joinable()) {
                    thread.join();
                    m_messageQueue.clear();
                }*/
                
            }
            catch (const std::exception &e) {
                std::cerr << "[Client] An error occurred during disconnection: " << e.what() << std::endl;
                // Handle exceptions as appropriate for your application
            }
        }
    }
} // moonshine
// net