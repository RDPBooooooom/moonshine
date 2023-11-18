//
// Created by marvin on 04.11.2023.
//

#include <boost/uuid/string_generator.hpp>
#include "Client.h"
#include "../editor/Scene.h"

namespace moonshine::net {

    void Client::connect(std::string ipv4, int64_t port) {
        std::cout << "[Client] Connecting..." << std::endl;
        m_connection->start(m_resolver.resolve(ipv4, std::to_string(port)));

        std::cout << "[Client] Connected!" << std::endl;
        
        m_connection.get()->async_receive_json();
        if (m_ioContext.stopped()) {
            m_ioContext.restart();
        }
        m_ioContextThread = std::thread([ObjectPtr = &m_ioContext] { return ObjectPtr->run(); });
        std::cout << "[Client] Context running" << std::endl;
    }

    void Client::disconnect() {
        if (m_connection->socket().is_open()) {
            try {
                // Close the connection
                m_connection->socket().close();

                // Stop the IO context to allow the thread to finish
                m_ioContext.stop();
                
                threadStop = true;

                // Join the thread if it's running
                if (m_ioContextThread.joinable()) {
                    m_ioContextThread.join();
                }

                std::cout << "[Client] Disconnected" << std::endl;
                m_messageQueue.notifyToStop();

                if (m_receiveThread.joinable()) {
                    m_receiveThread.join();
                    m_messageQueue.clear();
                }
                
            }
            catch (const std::exception &e) {
                std::cerr << "[Client] An error occurred during disconnection: " << e.what() << std::endl;
                // Handle exceptions as appropriate for your application
            }
        }
    }

    void Client::handleRequests() {
        while (!threadStop) {
            m_messageQueue.wait();
            if (threadStop) continue;

            boost::json::object jObj = m_messageQueue.pop_front().get_object();
            //std::cout << "[Client] Message received" << std::endl;
            resolver.resolve(jObj);
        }
        threadStop = false;
    }

    void Client::send(std::shared_ptr<SceneObject> &object) {
        boost::json::object jObj = object->getTransform()->serialize();
        jObj["action"] = "updateObject";
        jObj["objectId"] = object->get_id_as_string();
        m_connection->async_send_json(jObj);
        std::cout << "[Client] Message sent" << std::endl;
    }

    void Client::send(std::string &path, std::string &name, std::string &uuid) {
        boost::json::object jObj;
        jObj["action"] = "addObject";
        jObj["objectId"] = uuid;
        jObj["path"] = path;
        jObj["name"] = name;
        m_connection->async_send_json(jObj);
        std::cout << "[Client] Message sent" << std::endl;
    }
} // moonshine
// net