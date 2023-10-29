//
// Created by marvin on 28.10.2023.
//

#include "LobbyManager.h"
#include "imgui.h"

namespace moonshine {

    void LobbyManager::draw() {
        ImGui::Begin("Lobby manager");

        bool connected = connector.isConnected();
        if (connected) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Button("Connect", ImVec2(100, 20))) {
            connector.connect();
        };
        if (connected) {
            ImGui::EndDisabled();
        }
        
        if (!connected) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Button("Host", ImVec2(100, 20))) {
            connector.registerAsHost("Some Host");
        };
        if (ImGui::Button("Reload", ImVec2(100, 20))) {
            connector.receiveHosts();
        };
        if (!connected) {
            ImGui::EndDisabled();
        }
        
        connector.drawHosts();

        ImGui::End();
    }

} // moonshine