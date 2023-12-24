//
// Created by marvin on 04.11.2023.
//

#include <boost/uuid/string_generator.hpp>
#include "Client.h"
#include "../editor/Scene.h"
#include "../editor/EngineSystems.h"
#include "../editor/ui/net/UIManager.h"

namespace moonshine::net {

    void Client::connect(std::string ipv4, int64_t port) {
        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking, "[Client] Connecting...");
        m_connection->start(m_resolver.resolve(ipv4, std::to_string(port)));

        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking, "[Client] Connected!");

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

                threadStop = true;

                // Join the thread if it's running
                if (m_ioContextThread.joinable()) {
                    m_ioContextThread.join();
                }

                EngineSystems::getInstance().get_logger()->info(LoggerType::Networking, "[Client] Disconnected");
                m_messageQueue.notifyToStop();

                if (m_receiveThread.joinable()) {
                    m_receiveThread.join();
                    m_messageQueue.clear();
                }

            }
            catch (const std::exception &e) {
                EngineSystems::getInstance().get_logger()->info(LoggerType::Networking, 
                        "[Client] An error occurred during disconnection: {}", e.what());
            }
        }
    }

    void Client::handleRequests() {
        while (!threadStop) {
            m_messageQueue.wait();
            if (threadStop) continue;

            boost::json::object jObj = m_messageQueue.pop_front().get_object();
            resolver.resolve(jObj);
        }
        threadStop = false;
    }

    void Client::send(std::shared_ptr<SceneObject> &object) {
        boost::json::object jObj = object->getTransform()->serialize();
        jObj["action"] = "updateObject";
        jObj["objectId"] = object->get_id_as_string();
        m_connection->async_send_json(jObj);
        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                        std::string("[Client] UpdateObject message sent"));
    }

    void Client::send(std::string &path, std::string &name, std::string &uuid, Transform transform) {
        boost::json::object jObj;
        jObj["action"] = "addObject";
        jObj["objectId"] = uuid;
        jObj["path"] = path;
        jObj["name"] = name;
        jObj["transform"] = transform.serialize();
        m_connection->async_send_json(jObj);
        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                        std::string("[Client] AddObject message sent"));
    }

    void Client::send(std::string &label, element_locker locker) {
        boost::json::object jObj;
        jObj["action"] = "lockUI";
        jObj["label"] = label;
        jObj["locker"] = boost::json::to_value(locker);
        m_connection->async_send_json(jObj);
        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                        std::string("[Client] lockUi Message sent"));
    }

    void Client::send(std::string &uuid, std::string &name) {
        boost::json::object jObj;
        jObj["action"] = "renameObject";
        jObj["objectId"] = uuid;
        jObj["name"] = name;
        m_connection->async_send_json(jObj);
        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                        std::string("[Client] RenameObject message sent"));
    }

    void Client::send(std::string &uuid) {
        boost::json::object jObj;
        jObj["action"] = "removeObject";
        jObj["objectId"] = uuid;
        m_connection->async_send_json(jObj);
        EngineSystems::getInstance().get_logger()->info(LoggerType::Networking,
                                                        std::string("[Client] RemoveObject message sent"));
    }
} // moonshine
// net