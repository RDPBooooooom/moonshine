//
// Created by marvin on 28.10.2023.
//

#include "LobbyManager.h"
#include "imgui.h"

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
                    connector.registerAsHost(lobbyName);
                    connector.receiveHosts();
                }
            };
        }

        // Hosting and leaving a active session
        if (isHosting) {
            // Currently hosting a session 
            ImGui::SameLine();
            if (ImGui::Button("End Session", ImVec2(100, 20))) {
                std::cout << "TODO: End Session";
            };
        } else if (connected && !inSession && !isHosting) {
            // Connected to the lobby server, but not hosting or in a session
            ImGui::SameLine();
            if (ImGui::Button("Host", ImVec2(100, 20))) {
                openHostPrompt = true;
            };
            ImGui::SameLine();
            if (ImGui::Button("Join", ImVec2(100, 20))) {
                std::cout << "Join selected Session";
                inSession = true;
            };
            
        } else if (inSession) {
            // Currently connected to a host and therefor in a session
            ImGui::SameLine();
            if (ImGui::Button("Leave Session", ImVec2(100, 20))) {
                std::cout << "TODO: Leave Session";

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
                m_inputHandler->enable();

                connector.registerAsHost(lobbyName);
                connector.receiveHosts();

                isHosting = true;
            };
            lobbyName = text;

            if (ImGui::Button("Cancel")) {
                std::cout << "Cancel";
                ImGui::CloseCurrentPopup();
                m_inputHandler->enable();
            }

            ImGui::SameLine();
            ImGui::SetItemDefaultFocus();
            if (ImGui::Button("Start Hosting")) {
                ImGui::CloseCurrentPopup();
                m_inputHandler->enable();

                connector.registerAsHost(lobbyName);
                connector.receiveHosts();

                isHosting = true;
            }

            ImGui::EndPopup();
        }
    }

} // moonshine