//
// Created by marvin on 28.10.2023.
//

#include "LobbyManager.h"
#include "imgui.h"
#include "../Time.h"

namespace moonshine {

    void LobbyManager::draw() {
        ImGui::Begin("Lobby manager");

        bool connected = m_connector.is_connected();

        // Connect to the LobbyManager Server
        if (connected) {
            if (ImGui::Button("Disconnect", ImVec2(100, 20))) {
                m_connector.disconnect();
            };
        } else {
            if (ImGui::Button("Connect", ImVec2(100, 20))) {
                m_connector.try_connect();
                if (m_is_hosting) {
                    m_connector.register_as_host(m_lobby_name, m_server->get_port());
                    m_connector.receive_hosts();
                }
            };
        }

        // Hosting and leaving a active session
        if (m_is_hosting) {
            // Currently hosting a session 
            ImGui::SameLine();
            if (ImGui::Button("End Session", ImVec2(100, 20))) {
                m_server->stop();
                m_is_hosting = false;
            };
        } else if (connected && !m_in_session && !m_is_hosting) {
            // Connected to the lobby server, but not hosting or in a session
            ImGui::SameLine();
            if (ImGui::Button("Host", ImVec2(100, 20))) {
                m_open_host_prompt = true;
            };
            ImGui::SameLine();

            std::shared_ptr<LobbyConnector::Host> selected = m_connector.get_selected_host();
            ImGui::BeginDisabled(selected == nullptr);
            if (ImGui::Button("Join", ImVec2(100, 20))) {
                m_in_session = true;

                m_client->connect(selected->ipv4, selected->port);
            };
            ImGui::EndDisabled();

        } else if (m_in_session) {
            // Currently connected to a host and therefor in a session
            ImGui::SameLine();
            if (ImGui::Button("Leave Session", ImVec2(100, 20))) {
                m_client->disconnect();

                m_in_session = false;
            };
        }


        // Reload button
        if (!connected) {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Reload", ImVec2(100, 20))) {
            m_connector.receive_hosts();
        };

        if (!connected) {
            ImGui::EndDisabled();
        }

        m_connector.draw_hosts();

        ImGui::End();

        show_popup();
    }

    void LobbyManager::show_popup() {

        if (m_open_host_prompt) {
            ImGui::OpenPopup("Host?");
            m_inputHandler->disable();
            m_open_host_prompt = false;
        }

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Host?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            const int buffer_size = 256;
            char text[buffer_size] = {};
            std::string name = m_lobby_name;
            strncpy(text, name.c_str(), buffer_size - 1);
            text[buffer_size - 1] = '\0'; // Ensure null-termination
            ImGui::Text("Lobby name");
            ImGui::SameLine();

            if (ImGui::InputText("##lobbyName", text, buffer_size, ImGuiInputTextFlags_EnterReturnsTrue)) {
                m_lobby_name = text;
                ImGui::CloseCurrentPopup();
                start_hosting();
            };
            m_lobby_name = text;

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                m_inputHandler->enable();
            }

            ImGui::SameLine();
            ImGui::SetItemDefaultFocus();
            if (ImGui::Button("Start Hosting") && !m_lobby_name.empty()) {
                ImGui::CloseCurrentPopup();
                start_hosting();
            }

            ImGui::EndPopup();
        }
    }

    void LobbyManager::start_hosting() {

        m_inputHandler->enable();

        std::function<void()> start_hosting_server_handle = [this] {
            m_server = std::make_shared<net::Server>();

            m_connector.register_as_host(m_lobby_name, m_server->get_port());
            m_connector.receive_hosts();

            m_is_hosting = true;
        };
        std::thread thread(start_hosting_server_handle);
        thread.detach();
    }

    void LobbyManager::replicate() {
        m_time_since_last_replication += Time::s_delta_time;
        if (m_is_hosting && m_time_since_last_replication > 0.1f) {
            m_time_since_last_replication = 0;
            m_server->broadcast(Scene::get_current_scene());
        }
    }

    void LobbyManager::replicate(std::shared_ptr<SceneObject> &object) {
        if (m_in_session) {
            m_client->send(object);
        }
    }

    void LobbyManager::replicate_add(std::string path, std::string name, std::string uuid, Transform transform) {
        if (m_in_session) {
            m_client->send(path, name, uuid, transform);
        }
        if (m_is_hosting) {
            m_server->broadcast(path, name, uuid, transform);
        }
    }

    void LobbyManager::replicate_ui(std::string &label, element_locker &locker) {

        locker.last_replication = std::chrono::high_resolution_clock::now();

        if (m_in_session) {
            m_client->send(label, locker);
        }
        if (m_is_hosting) {
            m_server->broadcast(label, locker);
        }
    }

    void LobbyManager::replicate_remove(std::string uuid) {
        if (m_in_session) {
            m_client->send(uuid);
        }
        if (m_is_hosting) {
            m_server->broadcast(uuid);
        }
    }

    void LobbyManager::replicate_rename(std::string uuid, std::string name) {
        if (m_in_session) {
            m_client->send(uuid, name);
        }
        if (m_is_hosting) {
            m_server->broadcast(uuid, name);
        }
    }

} // moonshine