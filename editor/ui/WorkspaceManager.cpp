//
// Created by marvin on 16.11.2023.
//

#include "WorkspaceManager.h"
#include "imgui.h"
#include "../../external/ImGuiFileDialog/ImGuiFileDialog.h"
#include "../GltfLoader.h"
#include "../Scene.h"

namespace moonshine {
    void WorkspaceManager::draw() {
        ImGui::Begin("Workspace");
        drawInitModal();

        ImGui::SeparatorText(m_workspacePath.c_str());

        if (ImGui::Button("Import")) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseImport", "Choose a file to import", ".gltf", m_workspacePath + "/",
                                                    1,
                                                    nullptr, ImGuiFileDialogFlags_Modal);
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseImport")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string path = ImGuiFileDialog::Instance()->GetCurrentPath();
                std::string file = ImGuiFileDialog::Instance()->GetCurrentFileName();

                std::function<void()> importObject = [this, path, file] { import_object(path, file); };
                std::thread thread(importObject);
                thread.detach();
            }
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::End();
    }

    void WorkspaceManager::drawInitModal() {
        if (m_workspacePath.empty() && !m_workspaceModalActive) {
            m_workspaceModalActive = true;
            ImGui::OpenPopup("Select workspace");
            m_inputHandler->disable();
        }

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Select workspace", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

            if (ImGui::Button("Select folder")) {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose a workspace directory ", nullptr,
                                                        ".", 1,
                                                        nullptr, ImGuiFileDialogFlags_Modal);
            }
            ImGui::SameLine();
            ImGui::LabelText("##workspacePath", m_workspacePath.c_str());


            if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    m_workspacePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                }
                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGui::Button("Ok")) {
                if (!m_workspacePath.empty()) {
                    m_workspaceModalActive = false;
                    m_inputHandler->enable();
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

    void WorkspaceManager::import_object(std::string path, std::string file) {
        std::vector<std::shared_ptr<SceneObject>> toLoad = GltfLoader::load_gltf(path + "/", file);

        for (const auto &item: toLoad) {
            {
                auto lock = Scene::getCurrentScene().getLock();
                item->init(m_device, m_materialManager);
            }
            Scene::getCurrentScene().add_object(item);
        }
    }
} // moonshine