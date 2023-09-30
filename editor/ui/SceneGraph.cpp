﻿//
// Created by marvin on 30.09.2023.
//

#include <string>
#include "SceneGraph.h"
#include "imgui.h"

namespace moonshine {


    void SceneGraph::draw() {

        ImGui::Begin("Scene Graph");

        int index = 0;
        for (auto &item: *m_gameObjects) {
            std::string uniqueName = item->getName() + "##" + std::to_string(index++);

            bool isOpen = ImGui::TreeNode(uniqueName.c_str());
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Select")) {
                    m_selectedGameObject = item;
                }
                if (ImGui::MenuItem("Rename")) {
                    m_openPopup = true;
                    m_popupItem = item;
                }

                if (ImGui::MenuItem("Delete")) { /* Handle delete... */ }
                ImGui::EndPopup();
            }

            if (isOpen) {
                ImGui::TreePop();
            }

        }

        showPopup(m_popupItem);
        ImGui::End();

    }


    void SceneGraph::showPopup(std::shared_ptr<SceneObject> &item) {
        if (!item) return;

        if (m_openPopup) {
            ImGui::OpenPopup(("Rename " + item->getName()).c_str());
            m_inputHandler->disable();
            m_openPopup = false;
        }

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(("Rename " + item->getName()).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            const int bufferSize = 256;
            char text[bufferSize] = {};
            std::string name = item->getName();
            strncpy(text, name.c_str(), bufferSize - 1);
            text[bufferSize - 1] = '\0'; // Ensure null-termination
            ImGui::Text("Rename");
            ImGui::SameLine();
            ImGui::SetKeyboardFocusHere();
            if (ImGui::InputText("##objectName", text, bufferSize,
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string nameStr(text);
                item->setName(nameStr);
                ImGui::CloseCurrentPopup();

                m_popupItem = nullptr;
                m_inputHandler->enable();
            }
            ImGui::EndPopup();
        }
    }

    SceneGraph::SceneGraph(std::shared_ptr<std::vector<std::shared_ptr<SceneObject>>> &gameObjects,
                           std::shared_ptr<InputHandler> &inputHandler) : m_gameObjects{gameObjects},
                                                                          m_inputHandler{inputHandler} {
    }


} // moonshine