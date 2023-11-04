//
// Created by marvin on 28.10.2023.
//

#ifndef MOONSHINE_LOBBYCONNECTOR_H
#define MOONSHINE_LOBBYCONNECTOR_H

#include "TcpConnection.h"
#include "imgui.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace moonshine {

    class LobbyConnector {

    public:
        struct Host {
            int64_t id;
            std::string name;
            std::string ipv4;
            int64_t port;
        };

    private:

        boost::asio::io_context io_service;
        tcp::resolver resolver;
        TcpConnection::pointer connection;
        bool m_isConnected = false;
        SafeQueue<boost::json::value> messageQueue;

        bool threadStop = false;
        std::thread thread;

        std::thread ioContextThread;

        std::mutex hostMutex;
        std::unique_ptr<std::vector<Host>> currentHosts;

    public:

        LobbyConnector() : io_service(), resolver(io_service) {
            connection =
                    TcpConnection::create(io_service, messageQueue);

            currentHosts = std::make_unique<std::vector<Host>>();
        }

        ~LobbyConnector() {
            if (thread.joinable()) {
                thread.join();
            }
            if (ioContextThread.joinable()) {
                ioContextThread.join();
            }
        }

        void handleRequests() {
            while (isConnected() || !threadStop) {
                messageQueue.wait();
                if (!isConnected() || threadStop) continue;

                boost::json::object jObj = messageQueue.pop_front().get_object();
                std::cout << "Message found" << std::endl;

                boost::json::string action = jObj["action"].get_string();
                if (std::equal(action.begin(), action.end(), "updateHosts")) {
                    boost::json::array hosts = jObj["hosts"].as_array();
                    hostMutex.lock();
                    currentHosts->clear();
                    for (boost::json::value v: hosts) {
                        boost::json::object o = v.get_object();
                        Host host = {};
                        host.id = o["id"].get_int64();
                        host.name = o["name"].get_string();
                        host.ipv4 = o["ip"].get_string();
                        host.port = o["port"].get_int64();

                        currentHosts->push_back(host);
                    }
                    hostMutex.unlock();
                } else if (std::equal(action.begin(), action.end(), "join")) {
                    
                }
            }
            threadStop = false;
        }

        void drawHosts() {
            std::scoped_lock<std::mutex> lock(hostMutex);

            static int item_current_idx = 0;
            if (ImGui::BeginListBox("Available Hosts", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
                for (int n = 0; n < currentHosts->size(); n++) {
                    const bool is_selected = (item_current_idx == n);
                    if (ImGui::Selectable(currentHosts->at(n).name.c_str(), is_selected))
                        item_current_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }
        }

        bool isConnected() const {
            return m_isConnected;
        }

        void connect();

        void registerAsHost(std::string name);

        void receiveHosts();

        void disconnect();
    };

} // moonshine

#endif //MOONSHINE_LOBBYCONNECTOR_H
