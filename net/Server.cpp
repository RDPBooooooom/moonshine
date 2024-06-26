﻿//
// Created by marvin on 04.11.2023.
//

#include "Server.h"
#include "../editor/EngineSystems.h"

namespace moonshine::net {

    void Server::start_accept() {
        TcpConnection::pointer new_connection =
                TcpConnection::create(m_ioContext, m_messageQueue);

        EngineSystems::get_instance().get_logger()->info(LoggerType::Networking, "[SERVER] Waiting for connections...");

        m_acceptor.async_accept(new_connection->socket(),
                                bind(&Server::on_accept, this, new_connection,
                                     boost::asio::placeholders::error));
    }

    void Server::on_accept(TcpConnection::pointer &new_connection, const error_code &error) {
        if (!error) {

            EngineSystems::get_instance().get_logger()->info(LoggerType::Networking,
                                                             "[SERVER] New Connection by {}:{}",
                                                             new_connection->socket().remote_endpoint().address().to_string(),
                                                             std::to_string(
                                                                     new_connection->socket().remote_endpoint().port()));

            new_connection->start();

            m_clients.lock();
            m_connectedClients.push_back(new_connection);
            m_clients.unlock();

        }

        start_accept();
    }

    void Server::stop() {

        m_thread_stop = true;
        m_messageQueue.notify_to_stop();
        
        // Stop the IO context to allow the thread to finish
        m_ioContext.stop();

        // Join the thread if it's running
        if (m_ioThread.joinable()) {
            m_ioThread.join();
        }
        
        if(m_receiveThread.joinable()){
            m_receiveThread.join();
        }
        
        m_clients.lock();

        free_upnp();

        // Close the connection
        for (const auto &item: m_connectedClients) {
            item->socket().close();
        }
        m_connectedClients.clear();
        EngineSystems::get_instance().get_logger()->info(LoggerType::Networking, "[SERVER] Closed all connections");
        m_clients.unlock();
    }

    void Server::handle_requests() {
        while (!m_thread_stop) {
            m_messageQueue.wait();
            if (m_thread_stop) continue;

            boost::json::object jObj = m_messageQueue.pop_front().get_object();
            EngineSystems::get_instance().get_logger()->info(LoggerType::Networking, "[SERVER] Message received");
            m_request_resolver.resolve(jObj);
        }
        m_thread_stop = false;
    }

    void Server::do_upnp() {
        int error = 0;

        m_device_list = upnpDiscover(2000, nullptr, nullptr, 0, 0, 2, &error);
        char lan_ip[16];

        if (m_device_list) {
            if (UPNP_GetValidIGD(m_device_list, &m_urls, &m_data, lan_ip, sizeof(lan_ip)) == 1) {
                EngineSystems::get_instance().get_logger()->info(LoggerType::Networking, m_urls.controlURL);
                EngineSystems::get_instance().get_logger()->info(LoggerType::Networking, m_data.presentationurl);

                std::string port = std::to_string(get_port());
                // Expire after 4 hours
                if (UPNP_AddPortMapping(m_urls.controlURL, m_data.first.servicetype,
                                        port.c_str(), port.c_str(), lan_ip, "Moonshine P2P Server Host",
                                        "TCP", nullptr, "14400") != UPNPCOMMAND_SUCCESS) {
                    // Handle error
                    EngineSystems::get_instance().get_logger()->error(LoggerType::Networking,
                                                                      "[SERVER] error when mapping");
                }
                // Remember to free the URLs and device list when done
            }
        } else {
            // Handle error if no devices were found
            EngineSystems::get_instance().get_logger()->error(LoggerType::Networking,
                                                              "[SERVER] No device found for upnp");
        }
    }

    void Server::free_upnp() {

        if (m_was_upnp_freed) return;


        std::string port = std::to_string(get_port());

        int result = UPNP_DeletePortMapping(m_urls.controlURL, m_data.first.servicetype,
                                            port.c_str(), "TCP", nullptr);
        if (result != UPNPCOMMAND_SUCCESS) {

            // Handle the error
            EngineSystems::get_instance().get_logger()->error(LoggerType::Networking,
                                                              "[SERVER] unable to delete mapping");
            return;
        }

        FreeUPNPUrls(&m_urls);
        freeUPNPDevlist(m_device_list);

        m_was_upnp_freed = true;
        EngineSystems::get_instance().get_logger()->debug(LoggerType::Networking,
                                                          "[SERVER] UPNP freed");
    }

    void Server::broadcast(Scene &scene) {
        auto lock = scene.get_lock();

        boost::json::array objects;

        int i = 0;
        for (auto obj: scene) {
            boost::json::object jObj = obj->get_transform()->serialize();
            jObj["action"] = "updateObject";
            jObj["objectId"] = obj->get_id_as_string();
            objects.push_back(jObj);
            i++;
        }

        boost::json::object message;
        message["action"] = "updateScene";
        message["sceneObjects"] = objects;

        std::scoped_lock<std::mutex> client_lock(m_clients);
        for (auto client: m_connectedClients) {
            client->async_send_json(message);
        }
    }

    void Server::broadcast(std::string &path, std::string &name, std::string &uuid, Transform transform) {
        boost::json::object message;
        message["action"] = "addObject";
        message["objectId"] = uuid;
        message["path"] = path;
        message["name"] = name;
        message["transform"] = transform.serialize();

        for (auto client: m_connectedClients) {
            client->async_send_json(message);
        }
    }

    void Server::broadcast(std::string &label, element_locker locker) {
        boost::json::object message;
        message["action"] = "lockUI";
        message["label"] = label;
        message["locker"] = boost::json::to_value(locker);

        for (auto client: m_connectedClients) {
            client->async_send_json(message);
        }
    }

    void Server::broadcast(std::string &uuid) {
        boost::json::object message;
        message["action"] = "removeObject";
        message["objectId"] = uuid;

        for (auto client: m_connectedClients) {
            client->async_send_json(message);
        }
    }

    void Server::broadcast(std::string &uuid, std::string &name) {
        boost::json::object message;
        message["action"] = "renameObject";
        message["objectId"] = uuid;
        message["name"] = name;

        for (auto client: m_connectedClients) {
            client->async_send_json(message);
        }
    }

} // moonshine
// net