//
// Created by marvin on 16.11.2023.
//

#include "WorkspaceManager.h"

#include <utility>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/random_generator.hpp>
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

        ImGui::SameLine();

        if (ImGui::Button("Save Scene")) {
            save_scene();
        }

        ImGui::Separator();

        if (ImGuiFileDialog::Instance()->Display("ChooseImport")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string path = ImGuiFileDialog::Instance()->GetCurrentPath();
                std::string file = ImGuiFileDialog::Instance()->GetCurrentFileName();

                Transform transform = {};
                transform.position = m_camera.getTransform()->position + m_camera.getTransform()->getForward() * -5.0f;

                import_object(path + "\\", file, transform);
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

                    load_workspace_scene();
                }
            }

            ImGui::EndPopup();
        }
    }

    void WorkspaceManager::import_object(std::string path, std::string file, Transform transform) {
        import_data data = {};
        data.path = std::move(path);
        data.file = std::move(file);
        data.uuid = boost::uuids::uuid();

        data.overwrite_pos = true;

        data.transform = transform;

        import_object(data);
    }

    void
    WorkspaceManager::import_object(std::string path, std::string file, boost::uuids::uuid uuid, Transform transform) {
        import_data data = {};
        data.path = std::move(path);
        data.file = std::move(file);
        data.uuid = uuid;
        data.transform = transform;

        data.overwrite_pos = true;

        import_object(data);
    }

    void WorkspaceManager::import_object(import_data &data) {
        if (Scene::getCurrentScene().get_by_id(data.uuid) != nullptr ||
            std::find(m_generated_ids.begin(), m_generated_ids.end(), data.uuid) != m_generated_ids.end()) {
            return;
        }

        std::function<void()> importObject = [this, data] { import_object_gltf(data); };
        std::thread thread(importObject);
        thread.detach();
    }

    const std::string &WorkspaceManager::get_workspace_path() const {
        return m_workspacePath;
    }

    void WorkspaceManager::import_object_gltf(import_data data) {

        try {
            if (data.uuid.is_nil()) {
                data.uuid = boost::uuids::random_generator()();
            }
            m_generated_ids.push_back(data.uuid);

            std::string pathInWorkspace = FileUtils::get_relative_path(data.path, m_workspacePath);
            EngineSystems::getInstance().get_lobby_manager()->replicateAdd(pathInWorkspace, data.file,
                                                                           to_string(data.uuid), data.transform);

            std::vector<std::shared_ptr<SceneObject>> toLoad = GltfLoader::load_gltf(data.path, data.file,
                                                                                     data.uuid);

            for (const auto &item: toLoad) {
                {
                    auto lock = Scene::getCurrentScene().getLock();
                    item->init(m_device, m_materialManager);
                }

                if (!data.name.empty()) item->setName(data.name);
                if (data.overwrite_pos) item->getTransform()->position = data.transform.position;
                if (data.overwrite_rot) item->getTransform()->rotation = data.transform.rotation;
                if (data.overwrite_scale) item->getTransform()->scale = data.transform.scale;

                Scene::getCurrentScene().add_object(item);
            }

        } catch (const std::runtime_error &e) {
            EngineSystems::getInstance().get_logger()->error(LoggerType::Editor,
                                                             "Failed to import gltf ({})", e.what());
        } catch (const std::exception &e) {
            EngineSystems::getInstance().get_logger()->error(LoggerType::Editor,
                                                             "Failed to import gltf ({})", e.what());
        }
    }

    void WorkspaceManager::save_scene() {
        auto &scene = Scene::getCurrentScene();

        std::ofstream output_file(m_workspacePath + "\\scene.json");
        if (output_file.is_open()) {
            output_file << scene.serialize();
            output_file.close();
            EngineSystems::getInstance().get_logger()->debug(LoggerType::Editor,
                                                             "Saved scene.json");
        } else {
            EngineSystems::getInstance().get_logger()->error(LoggerType::Editor,
                                                             "Unable to save scene.json!");
        }
    }

    void WorkspaceManager::load_workspace_scene() {
        auto logger = EngineSystems::getInstance().get_logger();
        try {
            std::ifstream scene_file(m_workspacePath + "\\scene.json");
            if (scene_file.is_open()) {
                // File exists, read the settings
                std::string json_data((std::istreambuf_iterator<char>(scene_file)),
                                      std::istreambuf_iterator<char>());
                try {
                    auto jObj = boost::json::parse(json_data).as_object();

                    if (jObj.contains("objects")) {
                        handle_load_scene(jObj["objects"].as_array());
                    } else {
                        logger->warn(LoggerType::Editor, "scene.json doesn't contain any objects");
                    }

                    logger->info(LoggerType::Editor,
                                 "Found and loaded scene.json");
                } catch (const boost::json::system_error &jse) {
                    logger->error(LoggerType::Editor,
                                  "Error while loading scene.json. The file is probably in a wrong state!");
                }
            } else {
                logger->info(LoggerType::Editor,
                             "No scene file found, init with empty scene.");
            }

            scene_file.close();
        } catch (const std::ifstream::failure &ife) {
            logger->error(LoggerType::Editor,
                          "Error while reading scene.json file. Loaded empty scene.");
        }
    }

    void WorkspaceManager::handle_load_scene(const boost::json::array &objects) {

        boost::uuids::string_generator gen;

        for (const auto &item: objects) {

            auto sceneObject = item.as_object();

            import_data data = {};

            data.path = sceneObject["path"].as_string().c_str();
            data.file = sceneObject["filename"].as_string().c_str();
            data.uuid = gen(sceneObject["objectId"].as_string().c_str());
            data.name = sceneObject["name"].as_string().c_str();

            data.overwrite_pos = true;
            data.overwrite_rot = true;
            data.overwrite_scale = true;
            data.transform.deserialize(sceneObject["transform"].as_object());

            import_object(data);
        }
    }
} // moonshine