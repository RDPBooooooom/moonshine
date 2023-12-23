//
// Created by marvin on 28.10.2023.
//

#include <thread>
#include "LobbyConnector.h"
#include "../editor/EngineSystems.h"
#include "../MoonshineApp.h"

namespace moonshine {

    void LobbyConnector::registerAsHost(const std::string &name, int port) {

        boost::json::object object;
        object["action"] = "host";
        object["name"] = name;
        object["port"] = port;
        connection->async_send_json(object);
    }

    void LobbyConnector::try_connect() {
        std::function<void()> connect_handle = [this] { connect(); };
        std::thread connect_thread(connect_handle);
        connect_thread.detach();
    }

    void LobbyConnector::connect() {
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(MoonshineApp::APP_SETTINGS.LOBBY_SERVER_ADDRESS,
                                                                     MoonshineApp::APP_SETTINGS.LOBBY_SERVER_PORT);

        try {
            connection->start(endpoint_iterator);
        } catch (const boost::system::system_error &e) {
            EngineSystems::getInstance().get_logger()->error(LoggerType::Networking,
                                                             std::string("Failed to connect: ") + e.what());
            return;
        }


        std::function<void()> handleRequestsHandle = [this] { handleRequests(); };
        thread = std::thread(handleRequestsHandle);

        m_isConnected = true;

        connection.get()->async_receive_json();
        if (io_service.stopped()) {
            io_service.restart();
        }
        ioContextThread = std::thread([ObjectPtr = &io_service] { return ObjectPtr->run(); });

        boost::json::object object;
        object["action"] = "connect";
        connection.get()->async_send_json(object);
    }

    void LobbyConnector::receiveHosts() {
        boost::json::object object;
        object["action"] = "getHosts";

        connection.get()->async_send_json(object);
    }

    void LobbyConnector::disconnect() {
        if (m_isConnected) {
            try {
                // Close the connection
                connection->socket().close();

                // Stop the IO context to allow the thread to finish
                io_service.stop();

                // Join the thread if it's running
                if (ioContextThread.joinable()) {
                    ioContextThread.join();
                }

                m_isConnected = false;
                threadStop = true;

                EngineSystems::getInstance().get_logger()->info(LoggerType::Networking, std::string("Disconnected"));
                m_messageQueue.notifyToStop();

                if (thread.joinable()) {
                    thread.join();
                    m_messageQueue.clear();
                }

                std::scoped_lock<std::mutex> lock(hostMutex);
                currentHosts.clear();
            }
            catch (const std::exception &e) {
                std::cerr << "An error occurred during disconnection: " << e.what() << std::endl;
                // Handle exceptions as appropriate for your application
            }
        }
    }

    std::shared_ptr<LobbyConnector::Host> LobbyConnector::getSelectedHost() {
        std::scoped_lock<std::mutex> lock(hostMutex);

        if(item_current_idx < 0) return nullptr;
        if(item_current_idx >= currentHosts.size()) return nullptr;
        
        return currentHosts.at(item_current_idx);
    }

    void LobbyConnector::handleRequests() {
        while (isConnected() || !threadStop) {
            m_messageQueue.wait();
            if (!isConnected() || threadStop) continue;

            boost::json::object jObj = m_messageQueue.pop_front().get_object();

            boost::json::string action = jObj["action"].get_string();
            if (std::equal(action.begin(), action.end(), "updateHosts")) {
                boost::json::array hosts = jObj["hosts"].as_array();
                hostMutex.lock();
                currentHosts.clear();
                for (boost::json::value v: hosts) {
                    boost::json::object o = v.get_object();
                    std::shared_ptr<Host> host = std::make_shared<Host>();
                    host->id = o["id"].get_int64();
                    host->name = o["name"].get_string();
                    host->ipv4 = o["ip"].get_string();
                    host->port = o["port"].get_int64();

                    currentHosts.push_back(host);
                }
                hostMutex.unlock();
            }
        }
        threadStop = false;
    }

    void LobbyConnector::drawHosts() {
        std::scoped_lock<std::mutex> lock(hostMutex);

        if (ImGui::BeginListBox("Available Hosts", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (int n = 0; n < currentHosts.size(); n++) {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(currentHosts.at(n)->name.c_str(), is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
    }

} // moonshine