//
// Created by marvin on 29.05.2023.
//

#include "MoonshineApp.h"
#include "editor/InputHandler.h"
#include "editor/Time.h"
#include "editor/ui/net/InputFloat3.h"

namespace moonshine {

    //Settings MoonshineApp::APP_SETTINGS;

    MoonshineApp::MoonshineApp() : m_camera{Camera(app.get_input_handler())} {
    }

    void MoonshineApp::run() {
        //EASY_MAIN_THREAD;
        //EASY_PROFILER_ENABLE;

        main_loop();
        cleanup();
    }

    // MoonshineTodo: Bring back settings
    /***
    void MoonshineApp::load_settings() {
        try {

            std::ifstream settings_file("settings.json");
            if (settings_file.is_open()) {
                // File exists, read the settings
                std::string json_data((std::istreambuf_iterator<char>(settings_file)),
                                      std::istreambuf_iterator<char>());
                try {
                    MoonshineApp::APP_SETTINGS = Settings::deserialize(json_data);
                    EngineSystems::get_instance().get_logger()->info(LoggerType::Editor,
                                                                     "Found and loaded settings.json");
                } catch (const boost::json::system_error &jse) {
                    MoonshineApp::APP_SETTINGS = Settings{}; // The standard values are set from the struct initialization here.
                    EngineSystems::get_instance().get_logger()->error(LoggerType::Editor,
                                                                      "Error while loading json. The file is probably in a wrong state! Loaded with standard settings");
                }
            } else {
                // File does not exist, set default settings and write them to the file
                MoonshineApp::APP_SETTINGS = Settings{}; // The standard values are set from the struct initialization here.
                save_settings();
                EngineSystems::get_instance().get_logger()->info(LoggerType::Editor,
                                                                 "No settings file found, init with standard settings.json");
            }

            settings_file.close();
        } catch (const std::ifstream::failure &ife) {
            MoonshineApp::APP_SETTINGS = Settings{};
            EngineSystems::get_instance().get_logger()->error(LoggerType::Editor,
                                                              "Error while reading settings.json file. Loaded with standard settings");

        }
    }

    void MoonshineApp::save_settings() {
        std::string json_data = Settings::serialize(MoonshineApp::APP_SETTINGS);

        std::ofstream output_file("settings.json");
        if (output_file.is_open()) {
            output_file << json_data;
            output_file.close();
            EngineSystems::get_instance().get_logger()->debug(LoggerType::Editor,
                                                              "Saved settings.json");
        } else {
            EngineSystems::get_instance().get_logger()->error(LoggerType::Editor,
                                                              "Unable to save settings.json. Settings will not persist!");
        }
    }*/

    /*** MoonshineTodo: Remove => Old Vulkan
    void MoonshineApp::init_vulkan() {
        m_global_pool = DescriptorPool::Builder(m_device).set_max_sets(MAX_FRAMES_IN_FLIGHT)
                .add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT * 2)
                .build();

        m_materialManager = std::make_unique<MaterialManager>(m_device);

        m_matrixUBO.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < m_matrixUBO.size(); i++) {
            m_matrixUBO[i] = std::make_unique<Buffer>(
                    m_device,
                    sizeof(UniformBufferObject),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                    m_device.properties.limits.minUniformBufferOffsetAlignment
            );
            m_matrixUBO[i]->map();
        }

        EngineSystems::get_instance().get_logger()->debug(LoggerType::Rendering, "UBO created");

        m_fragUBO.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < m_matrixUBO.size(); i++) {
            m_fragUBO[i] = std::make_unique<Buffer>(
                    m_device,
                    sizeof(FragmentUniformBufferObject),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                    m_device.properties.limits.minUniformBufferOffsetAlignment
            );
            m_fragUBO[i]->map();
        }

        EngineSystems::get_instance().get_logger()->debug(LoggerType::Rendering, "FRAG UBO created");
    }
    */

    void MoonshineApp::main_loop() {

        Time::init_time();

        /*** MoonshineTodo: Remove => Old Vulkan
         * auto globalSetLayout = DescriptorSetLayout::Builder(m_device)
                .add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .add_binding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();
        std::vector<VkDescriptorSet> globalDescriptorSets(MAX_FRAMES_IN_FLIGHT);
        

        for (int i = 0; i < globalDescriptorSets.size(); i++) {

            auto bufferInfo = m_matrixUBO[i]->descriptor_info();

            auto fragBufferInfo = m_fragUBO[i]->descriptor_info();

            DescriptorWriter(*globalSetLayout, *m_global_pool)
                    .write_buffer(0, &bufferInfo)
                    .write_buffer(1, &fragBufferInfo)
                    .build(globalDescriptorSets[i]);
        }
        
        SimpleRenderSystem simpleRenderSystem{m_device, m_renderer.get_swap_chain_render_pass(),
                                              globalSetLayout->get_descriptor_set_layout(),
                                              m_materialManager->get_material_layout()};
        */

        // Init UI
        auto inputHandler = app.get_input_handler();
        m_sceneGraph = std::make_unique<SceneGraph>(inputHandler, m_camera);
        EngineSystems::get_instance().set_lobby_manager(std::make_shared<LobbyManager>(inputHandler));


        EngineSystems::get_instance().set_workspace_manager(
                std::make_shared<WorkspaceManager>(m_device, m_materialManager, inputHandler, m_camera));

        while (!app.should_close()) {
            //EASY_BLOCK("Main Loop");
            EngineSystems::get_instance().get_statistics()->start_frame();
            Time::calc_delta_time();

            m_gui_renderer.new_frame();

            {
                //EASY_BLOCK("Process Input");
                glfwPollEvents();
                m_window.get_input_handler()->trigger_events();
            }

            {
                //EASY_BLOCK("Render UI");
                EngineSystems::get_instance().get_workspace_manager()->draw();
                EngineSystems::get_instance().get_lobby_manager()->draw();
                EngineSystems::get_instance().get_logger()->draw();
                EngineSystems::get_instance().get_statistics()->draw();

                m_sceneGraph->draw();
                m_camera.show_debug();
                show_inspector();
            }

            {
                //EASY_BLOCK("Draw Scene");
                std::unique_lock<std::mutex> sceneLock = Scene::get_current_scene().get_lock();
                /* MoonshineTodo: Remove => Old Vulkan
                if (auto commandBuffer = m_renderer.begin_frame()) {

                    uint32_t frameIndex = m_renderer.get_frame_index();
                    update_uniform_buffer(frameIndex);

                    m_renderer.begin_swap_chain_render_pass(commandBuffer);

                    FrameInfo frameInfo{
                            frameIndex,
                            Time::s_delta_time,
                            commandBuffer,
                            m_camera,
                            globalDescriptorSets[frameIndex],
                            m_materialManager->get_descriptor_set()};

                    simpleRenderSystem.render_game_objects(frameInfo, Scene::get_current_scene());

                    m_gui.render_frame(commandBuffer);

                    m_renderer.end_swap_chain_render_pass(commandBuffer);
                    m_renderer.end_frame();
                }
                 */
                m_gui_renderer.render_frame();
            }

            {
                //EASY_BLOCK("Replication")
                EngineSystems::get_instance().get_lobby_manager()->replicate();
                EngineSystems::get_instance().get_ui_manager()->update();
            }

            {
                //EASY_BLOCK("Statistics")
                EngineSystems::get_instance().get_statistics()->end_frame();
            }

        }
        //profiler::dumpBlocksToFile("main_loop_profiler_dump.prof");

        // MoonshineTodo: Remove => Old Vulkan
        // vkDeviceWaitIdle(m_device.get_vk_device());
    }


    void MoonshineApp::show_inspector() {
        ImGui::Begin("Inspector");
        std::shared_ptr<SceneObject> selected = m_sceneGraph->getSelected();
        if (selected != nullptr) {
            bool isDirty = false;

            ImGui::Text(selected->get_name().c_str());
            ImGui::Text(selected->get_id_as_string().c_str());

            ImGui::SeparatorText("Transform");
            if (net::ui::InputFloat3(("Position##" + std::string(selected->get_id_as_string())).c_str(), glm::value_ptr(
                    selected->get_transform()->position))) {
                isDirty = true;
            }

            glm::vec3 rotEulerAngles = glm::degrees(eulerAngles(selected->get_transform()->rotation));
            if (net::ui::InputFloat3(("Rotation##" + std::string(selected->get_id_as_string())).c_str(),
                                     glm::value_ptr(rotEulerAngles))) {
                selected->get_transform()->rotation = glm::quat(glm::radians(rotEulerAngles));
                isDirty = true;
            }

            if (net::ui::InputFloat3(("Scale##" + std::string(selected->get_id_as_string())).c_str(), glm::value_ptr(
                    selected->get_transform()->scale))) {
                isDirty = true;
            }

            //MoonshineTodo: Reintroduce Material manager
            /*for (const auto &node: selected->get_nodes()) {
                for (const auto &mesh: node->get_sub_meshes()) {
                    m_materialManager->get_material(mesh.m_materialIdx)->draw_gui();
                }
            }*/

            if (isDirty) {
                EngineSystems::get_instance().get_lobby_manager()->replicate(selected);
            }
        }
        ImGui::End();
    }
} // moonshined