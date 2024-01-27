//
// Created by marvin on 28.10.2023.
//

#include <thread>
#include "LobbyConnector.h"
#include "../editor/EngineSystems.h"
#include "../MoonshineApp.h"

namespace moonshine {

    void LobbyConnector::register_as_host(const std::string &name, int port) {

        boost::json::object object;
        object["action"] = "host";
        object["name"] = name;
        object["port"] = port;
        m_connection->async_send_json(object);
    }

    void LobbyConnector::try_connect() {
        std::function<void()> connect_handle = [this] { connect(); };
        std::thread connect_thread(connect_handle);
        connect_thread.detach();
    }

    void LobbyConnector::connect() {
        tcp::resolver::iterator endpoint_iterator = m_resolver.resolve(MoonshineApp::APP_SETTINGS.LOBBY_SERVER_ADDRESS,
                                                                       MoonshineApp::APP_SETTINGS.LOBBY_SERVER_PORT);

        try {
            m_connection->start(endpoint_iterator);
        } catch (const boost::system::system_error &e) {
            EngineSystems::get_instance().get_logger()->error(LoggerType::Networking,
                                                              "Failed to connect: {}", e.what());
            return;
        }


        std::function<void()> handle_requests_handle = [this] { handle_requests(); };
        m_thread = std::thread(handle_requests_handle);

        m_isConnected = true;

        m_connection.get()->async_receive_json();
        if (m_io_service.stopped()) {
            m_io_service.restart();
        }
        m_io_context_thread = std::thread([ObjectPtr = &m_io_service] { return ObjectPtr->run(); });

        boost::json::object object;
        object["action"] = "connect";
        m_connection.get()->async_send_json(object);
    }

    void LobbyConnector::receive_hosts() {
        boost::json::object object;
        object["action"] = "getHosts";

        m_connection.get()->async_send_json(object);
    }

    void LobbyConnector::disconnect() {
        if (m_isConnected) {
            try {
                // Close the connection
                m_connection->socket().close();

                // Stop the IO context to allow the thread to finish
                m_io_service.stop();

                // Join the thread if it's running
                if (m_io_context_thread.joinable()) {
                    m_io_context_thread.join();
                }

                m_isConnected = false;
                m_thread_stop = true;

                EngineSystems::get_instance().get_logger()->info(LoggerType::Networking, std::string("Disconnected"));
                m_messageQueue.notify_to_stop();

                if (m_thread.joinable()) {
                    m_thread.join();
                    m_messageQueue.clear();
                }

                std::scoped_lock<std::mutex> lock(m_host_mutex);
                m_current_hosts.clear();
            }
            catch (const std::exception &e) {
                std::cerr << "An error occurred during disconnection: " << e.what() << std::endl;
            }
        }
    }

    std::shared_ptr<LobbyConnector::Host> LobbyConnector::get_selected_host() {
        std::scoped_lock<std::mutex> lock(m_host_mutex);

        if(m_item_current_idx < 0) return nullptr;
        if(m_item_current_idx >= m_current_hosts.size()) return nullptr;
        
        return m_current_hosts.at(m_item_current_idx);
    }

    void LobbyConnector::handle_requests() {
        while (is_connected() || !m_thread_stop) {
            m_messageQueue.wait();
            if (!is_connected() || m_thread_stop) continue;

            boost::json::object jObj = m_messageQueue.pop_front().get_object();

            boost::json::string action = jObj["action"].get_string();
            if (std::equal(action.begin(), action.end(), "updateHosts")) {
                boost::json::array hosts = jObj["hosts"].as_array();
                m_host_mutex.lock();
                m_current_hosts.clear();
                for (boost::json::value v: hosts) {
                    boost::json::object o = v.get_object();
                    std::shared_ptr<Host> host = std::make_shared<Host>();
                    host->id = o["id"].get_int64();
                    host->name = o["name"].get_string();
                    host->ipv4 = o["ip"].get_string();
                    host->port = o["port"].get_int64();

                    m_current_hosts.push_back(host);
                }
                m_host_mutex.unlock();
            }
        }
        m_thread_stop = false;
    }

    void LobbyConnector::draw_hosts() {
        std::scoped_lock<std::mutex> lock(m_host_mutex);

        if (ImGui::BeginListBox("Available Hosts", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (int n = 0; n < m_current_hosts.size(); n++) {
                const bool is_selected = (m_item_current_idx == n);
                if (ImGui::Selectable(m_current_hosts.at(n)->name.c_str(), is_selected))
                    m_item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
    }

} // moonshine