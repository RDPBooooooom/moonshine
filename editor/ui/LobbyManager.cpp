﻿//
// Created by marvin on 28.10.2023.
//

#include "LobbyManager.h"
#include "imgui.h"
#include "../Time.h"

namespace moonshine {

    void LobbyManager::draw() {
        ImGui::Begin("Lobby manager");

        bool connected = connector.isConnected();

        // Connect to the LobbyManager Server
        if (connected) {
            if (ImGui::Button("Disconnect", ImVec2(100, 20))) {
                connector.disconnect();
            };
        } else {
            if (ImGui::Button("Connect", ImVec2(100, 20))) {
                connector.connect();
                if (isHosting) {
                    connector.registerAsHost(lobbyName, m_server->get_port());
                    connector.receiveHosts();
                }
            };
        }

        // Hosting and leaving a active session
        if (isHosting) {
            // Currently hosting a session 
            ImGui::SameLine();
            if (ImGui::Button("End Session", ImVec2(100, 20))) {
                m_server->stop();
                isHosting = false;
            };
        } else if (connected && !inSession && !isHosting) {
            // Connected to the lobby server, but not hosting or in a session
            ImGui::SameLine();
            if (ImGui::Button("Host", ImVec2(100, 20))) {
                openHostPrompt = true;
            };
            ImGui::SameLine();
            if (ImGui::Button("Join", ImVec2(100, 20))) {
                inSession = true;

                LobbyConnector::Host selected = connector.getSelectedHost();

                m_client->connect(selected.ipv4, selected.port);
            };

        } else if (inSession) {
            // Currently connected to a host and therefor in a session
            ImGui::SameLine();
            if (ImGui::Button("Leave Session", ImVec2(100, 20))) {
                m_client->disconnect();

                inSession = false;
            };
        }


        // Reload button
        if (!connected) {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Reload", ImVec2(100, 20))) {
            connector.receiveHosts();
        };

        if (!connected) {
            ImGui::EndDisabled();
        }

        connector.drawHosts();

        ImGui::End();

        showPopup();
    }

    void LobbyManager::showPopup() {

        if (openHostPrompt) {
            ImGui::OpenPopup("Host?");
            m_inputHandler->disable();
            openHostPrompt = false;
        }

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Host?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            const int bufferSize = 256;
            char text[bufferSize] = {};
            std::string name = lobbyName;
            strncpy(text, name.c_str(), bufferSize - 1);
            text[bufferSize - 1] = '\0'; // Ensure null-termination
            ImGui::Text("Lobby name");
            ImGui::SameLine();

            if (ImGui::InputText("##lobbyName", text, bufferSize, ImGuiInputTextFlags_EnterReturnsTrue)) {
                lobbyName = text;
                ImGui::CloseCurrentPopup();
                start_hosting();
            };
            lobbyName = text;

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                m_inputHandler->enable();
            }

            ImGui::SameLine();
            ImGui::SetItemDefaultFocus();
            if (ImGui::Button("Start Hosting")) {
                ImGui::CloseCurrentPopup();
                start_hosting();
            }

            ImGui::EndPopup();
        }
    }

    void LobbyManager::start_hosting() {

        m_inputHandler->enable();

        m_server = std::make_shared<net::Server>();

        connector.registerAsHost(lobbyName, m_server->get_port());
        connector.receiveHosts();

        isHosting = true;
    }

    void LobbyManager::replicate() {
        timeSinceLastReplication += Time::deltaTime;
        if (isHosting && timeSinceLastReplication > 0.1f) {
            timeSinceLastReplication = 0;
            m_server->broadcast(Scene::getCurrentScene());
        }
    }

    void LobbyManager::replicate(std::shared_ptr<SceneObject> &object) {
        if (inSession) {
            m_client->send(object);
        }
    }

    void LobbyManager::replicateAdd(std::string path, std::string name, std::string uuid) {
        if (inSession) {
            m_client->send(path, name, uuid);
        }
        if (isHosting) {
            m_server->broadcast(path, name, uuid);
        }
    }

} // moonshine