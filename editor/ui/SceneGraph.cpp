//
// Created by marvin on 30.09.2023.
//

#include <string>
#include "SceneGraph.h"
#include "imgui.h"
#include "../Scene.h"
#include "thread"
#include "../EngineSystems.h"

namespace moonshine {

    void SceneGraph::draw() {

        ImGui::Begin("Scene Graph");

        int index = 0;
        Scene &current = Scene::get_current_scene();
        {
            auto lock = current.get_lock();
            for (auto &item: current) {
                std::string uniqueName = item->get_name() + "##" + std::to_string(index++);

                bool isOpen = ImGui::TreeNode(uniqueName.c_str());
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Select")) {
                        m_selected_game_object = item;
                    }
                    if (ImGui::MenuItem("Rename")) {
                        m_open_popup = true;
                        m_popup_item = item;
                    }

                    if (ImGui::MenuItem("Delete")) {
                        m_delete_item = item;
                    }
                    ImGui::EndPopup();
                }

                if (isOpen) {
                    ImGui::TreePop();
                }
            }
        }

        show_popup(m_popup_item);
        handle_delete(current, m_delete_item);

        ImGui::End();

    }


    void SceneGraph::show_popup(std::shared_ptr<SceneObject> &item) {
        if (!item) return;

        if (m_open_popup) {
            ImGui::OpenPopup(("Rename " + item->get_name()).c_str());
            m_input_handler->disable();
            m_open_popup = false;
        }

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(("Rename " + item->get_name()).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            const int buffer_size = 256;
            char text[buffer_size] = {};
            std::string name = item->get_name();
            strncpy(text, name.c_str(), buffer_size - 1);
            text[buffer_size - 1] = '\0'; // Ensure null-termination
            ImGui::Text("Rename");
            ImGui::SameLine();
            ImGui::SetKeyboardFocusHere();
            if (ImGui::InputText("##objectName", text, buffer_size,
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string nameStr(text);
                item->set_name(nameStr);
                ImGui::CloseCurrentPopup();

                EngineSystems::get_instance().get_lobby_manager()->replicate_rename(item->get_id_as_string(),
                                                                                    item->get_name());

                m_popup_item = nullptr;
                m_input_handler->enable();
            }
            ImGui::EndPopup();
        }
    }

    SceneGraph::SceneGraph(std::shared_ptr<InputHandler> &inputHandler, Camera &camera) : m_input_handler{inputHandler},
                                                                                          m_camera{camera} {

        using std::placeholders::_1;
        std::function<void(bool)> focus_handle = std::bind(&SceneGraph::focus_selected, this, _1);
        m_focus_selected_input_handle = inputHandler->register_key_event(GLFW_KEY_F, focus_handle, false, false);
    }

    void SceneGraph::handle_delete(Scene &scene, std::shared_ptr<SceneObject> item) {
        if (item == nullptr) return;

        if (item == m_selected_game_object) {
            m_selected_game_object = nullptr;
        }

        scene.remove_object(item);
        m_delete_item = nullptr;
    }

    void SceneGraph::focus_selected(bool isReleased) {
        if (m_selected_game_object == nullptr) return;

        m_camera.look_at(*m_selected_game_object->get_transform());

        EngineSystems::get_instance().get_logger()->debug(LoggerType::Editor,
                                                          "focused {}", m_selected_game_object->as_string());
    }

    SceneGraph::~SceneGraph() {
        m_input_handler->unregister_key_event(m_focus_selected_input_handle);
    }

} // moonshine