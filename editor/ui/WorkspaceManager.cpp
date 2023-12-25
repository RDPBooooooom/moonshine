//
// Created by marvin on 16.11.2023.
//

#include "WorkspaceManager.h"

#include <utility>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <filesystem>
#include "imgui.h"
#include "../../external/ImGuiFileDialog/ImGuiFileDialog.h"
#include "../GltfLoader.h"
#include "../Scene.h"
#include "../../utils/FileUtils.h"
#include "../EngineSystems.h"
#include "../../MoonshineApp.h"
#include "../../utils/FileUtils.h"

namespace moonshine {


    void WorkspaceManager::draw() {
        ImGui::Begin("Workspace");
        drawInitModal();

        ImGui::SeparatorText(m_workspacePath.c_str());

        if (ImGui::Button("Import into workspace")) {
            static std::string path;

            if (path.empty()) {
                try {
                    path = FileUtils::get_downloads_directory().string() + "\\";
                } catch (const std::runtime_error &err) {
                    path = "/";
                }
            }
            ImGuiFileDialog::Instance()->OpenDialog("ChooseImport", "Choose a file to import", nullptr,
                                                    path, 1,
                                                    nullptr, ImGuiFileDialogFlags_Modal);
        }

        ImGui::SameLine();

        if (ImGui::Button("Save Scene")) {
            save_scene();
        }

        ImGui::Separator();

        draw_workspace_items();

        if (ImGuiFileDialog::Instance()->Display("ChooseImport")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string path = ImGuiFileDialog::Instance()->GetCurrentPath();

                handle_workspace_import(path);
            }
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::End();
    }

    void WorkspaceManager::draw_workspace_items() {
        // Width of a button, adjust to your needs
        const float buttonSize = 75.0f;
        // Spacing between buttons, adjust to your needs
        const float buttonSpacing = 10.0f;

        //Number of buttons per row will be calculated dynamically based on window width
        int buttonsPerRow = ImGui::GetContentRegionAvail().x / (buttonSize + buttonSpacing);
        buttonsPerRow = std::max(buttonsPerRow, 1); // Make sure it's at least 1 to avoid division by zero

        int counter = 0;
        
        std::shared_ptr<workspace_object> to_delete = nullptr;

        for (const auto &workspace_object: m_available_imports) {
            ImGui::BeginGroup();
            if (ImGui::ImageButton((workspace_object->name + "##" + std::to_string(counter)).c_str(),
                                   workspace_object->has_thumbnail ? workspace_object->m_imGui_thumbnail_DS
                                                                   : m_imGui_placeHolder_DS,
                                   ImVec2(buttonSize, buttonSize))) {
                Transform transform = {};
                transform.position = m_camera.getTransform()->position + m_camera.getTransform()->getForward() * -5.0f;

                import_object(workspace_object->path, workspace_object->file, transform);
            }
            if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
            {
                if (ImGui::Button("Delete")) {
                    to_delete = workspace_object;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
            ImGui::TextDisabled((workspace_object->name).c_str());
            ImGui::EndGroup();

            // handling wrapping
            counter++;
            if (counter % buttonsPerRow != 0) {
                ImGui::SameLine();
            }
        }

        remove_workspace_object(to_delete);
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
                    discover_workspace();
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

    void WorkspaceManager::discover_workspace() {
        for (const auto &directory_search: std::filesystem::directory_iterator(m_workspacePath)) {
            if (directory_search.is_directory()) {
                for (const auto &file_search: std::filesystem::directory_iterator(directory_search.path())) {
                    if (file_search.is_regular_file() && file_search.path().extension() == ".gltf") {

                        std::shared_ptr<workspace_object> obj = load_workspace_object(
                                file_search.path().parent_path().string() + "\\",
                                file_search.path().filename().string(),
                                file_search.path().stem().string());

                        {
                            std::unique_lock<std::mutex> lock(m_available_imports_mtx);
                            m_available_imports.push_back(obj);
                        }
                    }
                }
            }
        }
    }

    std::shared_ptr<WorkspaceManager::workspace_object>
    WorkspaceManager::load_workspace_object(std::string path, std::string file, std::string name) {
        std::shared_ptr<workspace_object> obj = std::make_shared<workspace_object>();
        obj->path = std::move(path);
        obj->file = std::move(file);
        obj->name = std::move(name);

        if (std::filesystem::exists(obj->path + "thumbnail.png")) {
            auto lock = Scene::getCurrentScene().getLock();
            obj->m_thumbnail_image = std::make_unique<TextureImage>(
                    (obj->path + std::string("thumbnail.png")).c_str(), &m_device,
                    m_device.getCommandPool());
            obj->m_imGui_thumbnail_DS = ImGui_ImplVulkan_AddTexture(m_previewSampler->getVkSampler(),
                                                                    obj->m_thumbnail_image->getImageView(),
                                                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            obj->has_thumbnail = true;
        }

        return obj;
    }

    void WorkspaceManager::remove_workspace_object(std::shared_ptr<workspace_object> &toDelete) {
        if(toDelete == nullptr) return;
        {
            std::unique_lock<std::mutex> lock(m_available_imports_mtx);
            m_available_imports.erase(std::remove(m_available_imports.begin(), m_available_imports.end(), toDelete),
                                      m_available_imports.end());
        }
        
        // remove folder in workspace
        for (auto &dir_entry: std::filesystem::directory_iterator(toDelete->path)) {
            std::filesystem::remove(dir_entry);
        }
        std::filesystem::remove(toDelete->path);

    }

    void WorkspaceManager::handle_workspace_import(std::string &path) {
        auto logger = EngineSystems::getInstance().get_logger();

        bool found_gltf = false;
        std::string name;
        std::string filename;
        for (const auto &file_search: std::filesystem::directory_iterator(path)) {
            if (file_search.is_regular_file() && file_search.path().extension() == ".gltf") {
                if (found_gltf) {
                    logger->error(LoggerType::Editor,
                                  "Can't import folder because multiple gltf files were found in root!");
                    return;
                } else {
                    found_gltf = true;
                    name = file_search.path().stem().string();
                    filename = file_search.path().filename().string();
                }
            }
        }

        if (!found_gltf) {
            logger->error(LoggerType::Editor, "Can't import folder because no gltf file was found in root!");
            return;
        }

        std::string destination;
        std::string temp_folder;

        try {
            {
                std::unique_lock<std::mutex> lock(m_unique_temp_folder);
                validate_path(m_workspacePath + "\\temp\\", temp_folder, name);
                std::filesystem::create_directories(temp_folder);
            }


            // move to temp
            for (const auto &dir_entry: std::filesystem::directory_iterator(path)) {
                const auto &path = dir_entry.path();
                if (std::filesystem::is_regular_file(path)) {
                    std::filesystem::copy(path, temp_folder / path.filename());
                }
            }

        } catch (std::filesystem::filesystem_error &err) {
            logger->error(LoggerType::Editor, "Unable to import folder: {}", err.what());

            for (auto &dir_entry: std::filesystem::directory_iterator(temp_folder)) {
                std::filesystem::remove(dir_entry);
            }
            std::filesystem::remove(temp_folder);
            return;
        }

        try {
            {
                std::unique_lock<std::mutex> lock(m_unique_workspace_folder);
                validate_path(m_workspacePath + "\\", destination, name);
                std::filesystem::create_directories(destination);
            }

            for (const auto &dir_entry: std::filesystem::directory_iterator(temp_folder)) {
                const auto &path = dir_entry.path();
                if (std::filesystem::is_regular_file(path)) {
                    std::filesystem::rename(path,
                                            destination / path.filename()); // Move files from temp to final destination
                }
            }

            std::filesystem::remove(temp_folder);
        } catch (std::filesystem::filesystem_error &err) {
            logger->error(LoggerType::Editor, "Unable to import folder: {}", err.what());

            // Remove temp folder
            for (auto &dir_entry: std::filesystem::directory_iterator(temp_folder)) {
                std::filesystem::remove(dir_entry);
            }
            std::filesystem::remove(temp_folder);

            // remove folder in workspace
            for (auto &dir_entry: std::filesystem::directory_iterator(destination)) {
                std::filesystem::remove(dir_entry);
            }
            std::filesystem::remove(destination);

            return;
        }

        std::shared_ptr<workspace_object> obj = load_workspace_object(destination, filename, name);
        {
            std::unique_lock<std::mutex> lock(m_available_imports_mtx);
            m_available_imports.push_back(obj);
        }

        logger->info(LoggerType::Editor, "Imported {}", name);
    }

    void WorkspaceManager::validate_path(const std::string &root_path, std::string &path, const std::string &name,
                                         int iteration) {

        path = root_path + name + (iteration == 0 ? "\\" : "_" + std::to_string(iteration) + "\\");

        for (const auto &item: m_available_imports) {
            if (item->path == path) {
                validate_path(root_path, path, name, ++iteration);
                break;
            }
        }
    }

    void WorkspaceManager::clean_up() {
        m_available_imports.clear();
        m_placeholder_image = nullptr;
        m_previewSampler = nullptr;
    }
} // moonshine