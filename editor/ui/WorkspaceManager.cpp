//
// Created by marvin on 16.11.2023.
//

#include "WorkspaceManager.h"
#include "imgui.h"
#include "../../external/ImGuiFileDialog/ImGuiFileDialog.h"
#include "../GltfLoader.h"
#include "../Scene.h"
#include "../../utils/FileUtils.h"
#include "../EngineSystems.h"
#include "../../MoonshineApp.h"

namespace moonshine {
    void WorkspaceManager::draw() {
        ImGui::Begin("Workspace");
        drawInitModal();

        ImGui::SeparatorText(m_workspacePath.c_str());

        if (ImGui::Button("Import")) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseImport", "Choose a file to import", ".gltf",
                                                    m_workspacePath + "/",
                                                    1,
                                                    nullptr, ImGuiFileDialogFlags_Modal);
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseImport")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string path = ImGuiFileDialog::Instance()->GetCurrentPath();
                std::string file = ImGuiFileDialog::Instance()->GetCurrentFileName();

                import_object(path, file);
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

            m_workspacePath = MoonshineApp::APP_SETTINGS.LATEST_WORKSPACE;
        }

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Select workspace", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

            if (ImGui::Button("Select folder")) {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose a workspace directory ", nullptr,
                                                        m_workspacePath, 1,
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
                    MoonshineApp::APP_SETTINGS.LATEST_WORKSPACE = m_workspacePath;
                }
            }

            ImGui::EndPopup();
        }
    }

    void WorkspaceManager::import_object_gltf(std::string path, std::string file) {
        import_object_gltf(path, file, boost::uuids::uuid());
    }

    void WorkspaceManager::import_object(std::string path, std::string file) {
        std::function<void()> importObject = [this, path, file] { import_object_gltf(path, file); };
        std::thread thread(importObject);
        thread.detach();
    }

    const std::string &WorkspaceManager::get_workspace_path() const {
        return m_workspacePath;
    }

    void WorkspaceManager::import_object(std::string path, std::string file, boost::uuids::uuid uuid) {

        if (Scene::getCurrentScene().get_by_id(uuid) != nullptr) {
            return;
        }

        std::function<void()> importObject = [this, path, file, uuid] { import_object_gltf(path, file, uuid); };
        std::thread thread(importObject);
        thread.detach();
    }

    void WorkspaceManager::import_object_gltf(std::string path, std::string file, boost::uuids::uuid uuid) {

        try {
            std::vector<std::shared_ptr<SceneObject>> toLoad = GltfLoader::load_gltf(path + "/", file, uuid);

            std::string pathInWorkspace = FileUtils::get_relative_path(path, m_workspacePath);

            for (const auto &item: toLoad) {
                {
                    EngineSystems::getInstance().get_lobby_manager()->replicateAdd(pathInWorkspace, file,
                                                                                   item->get_id_as_string());

                    auto lock = Scene::getCurrentScene().getLock();
                    item->init(m_device, m_materialManager);
                }
                Scene::getCurrentScene().add_object(item);
            }

        } catch (const std::runtime_error &e) {
            EngineSystems::getInstance().get_logger()->error(LoggerType::Editor, "Failed to import gltf (" + std::string (e.what()) + ")");
        } catch (const std::exception &e) {
            EngineSystems::getInstance().get_logger()->error(LoggerType::Editor, "Failed to import gltf (" + std::string (e.what()) + ")");
        }
    }


} // moonshine