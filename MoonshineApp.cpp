//
// Created by marvin on 29.05.2023.
//

#include "MoonshineApp.h"
#include "external/imgui/imgui.h"
#include "external/imgui/backends/imgui_impl_glfw.h"
#include "external/imgui/backends/imgui_impl_vulkan.h"
#include "editor/Settings.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui_internal.h"
#include "editor/EngineSystems.h"
#include "editor/ui/net/InputFloat3.h"
#include "easy/profiler.h"


namespace moonshine {

    Settings MoonshineApp::APP_SETTINGS;

    static void check_vk_result(VkResult err) {
        if (err == 0)
            return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0)
            abort();
    }

    MoonshineApp::MoonshineApp() : m_camera{Camera(&m_window)} {
    }

    void MoonshineApp::run() {
        EASY_MAIN_THREAD;
        EASY_PROFILER_ENABLE;

        init_vulkan();
        init_im_gui();
        main_loop();
        cleanup();
    }


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
    }

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

    void MoonshineApp::init_im_gui() {
        VkDescriptorPoolSize pool_sizes[] =
                {
                        {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
                        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
                        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
                        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
                        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
                };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        check_vk_result(vkCreateDescriptorPool(m_device.get_vk_device(), &pool_info, nullptr, &m_imGuiPool));

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.Fonts->AddFontDefault();

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(m_window.get_glfw_window(), true);

        // Setup Platform/Renderer backends
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_device.get_vk_instance();
        init_info.PhysicalDevice = m_device.get_vk_physical_device();
        init_info.Device = m_device.get_vk_device();
        init_info.QueueFamily = m_device.get_graphics_queue_family();
        init_info.Queue = m_device.get_graphics_queue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = m_imGuiPool;
        init_info.Subpass = 0;
        init_info.MinImageCount = 2;
        init_info.ImageCount = 2;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = VK_NULL_HANDLE;
        init_info.CheckVkResultFn = check_vk_result;

        ImGui_ImplVulkan_Init(&init_info, m_renderer.get_swap_chain_render_pass());

        VkCommandBuffer commandBuffer = begin_single_time_commands(&m_device, m_device.get_command_pool());
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        end_single_time_commands(&m_device, commandBuffer, m_device.get_command_pool());

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    void MoonshineApp::main_loop() {

        Time::init_time();

        auto globalSetLayout = DescriptorSetLayout::Builder(m_device)
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

        // Init UI
        auto inputHandler = m_window.get_input_handler();
        m_sceneGraph = std::make_unique<SceneGraph>(inputHandler, m_camera);
        EngineSystems::get_instance().set_lobby_manager(std::make_shared<LobbyManager>(inputHandler));

        EngineSystems::get_instance().set_workspace_manager(
                std::make_shared<WorkspaceManager>(m_device, m_materialManager, inputHandler, m_camera));

        while (!m_window.should_close()) {
            EASY_BLOCK("Main Loop");
            EngineSystems::get_instance().get_statistics()->start_frame();
            Time::calc_delta_time();

            {
                EASY_BLOCK("ImGUI New Frame")
                ImGui_ImplVulkan_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                create_dock_space();
            }

            {
                EASY_BLOCK("Process Input");
                glfwPollEvents();
                m_window.get_input_handler()->trigger_events();
            }

            {
                EASY_BLOCK("Render UI");
                EngineSystems::get_instance().get_workspace_manager()->draw();
                EngineSystems::get_instance().get_lobby_manager()->draw();
                EngineSystems::get_instance().get_logger()->draw();
                EngineSystems::get_instance().get_statistics()->draw();

                m_sceneGraph->draw();
                m_camera.show_debug();
                show_inspector();
            }

            {
                EASY_BLOCK("Draw Scene");
                std::unique_lock<std::mutex> sceneLock = Scene::get_current_scene().get_lock();
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

                    ImGui::Render();
                    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

                    m_renderer.end_swap_chain_render_pass(commandBuffer);
                    m_renderer.end_frame();
                }
            }

            {
                EASY_BLOCK("Replication")
                EngineSystems::get_instance().get_lobby_manager()->replicate();
                EngineSystems::get_instance().get_ui_manager()->update();
            }

            {
                EASY_BLOCK("Statistics")
                EngineSystems::get_instance().get_statistics()->end_frame();
            }

        }
        profiler::dumpBlocksToFile("main_loop_profiler_dump.prof");

        vkDeviceWaitIdle(m_device.get_vk_device());
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

            for (const auto &node: selected->get_nodes()) {
                for (const auto &mesh: node->get_sub_meshes()) {
                    m_materialManager->get_material(mesh.m_materialIdx)->draw_gui();
                }
            }

            if (isDirty) {
                EngineSystems::get_instance().get_lobby_manager()->replicate(selected);
            }
        }
        ImGui::End();
    }


    void MoonshineApp::update_uniform_buffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                currentTime - startTime).count();

        UniformBufferObject ubo{};

        ubo.view = m_camera.get_view_mat();
        ubo.proj = glm::perspective(glm::radians(45.0f), m_renderer.get_swap_chain_extent().width /
                                                         (float) m_renderer.get_swap_chain_extent().height,
                                    0.1f,
                                    100.0f);

        /*
         * GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.
         * The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis 
         * in the projection matrix. If you don't do this, then the image will be rendered upside down.
         */
        ubo.proj[1][1] *= -1;

        MaterialData material{};
        DirLight light{};
        light.direction = glm::normalize(glm::vec3(0, 1, -1));
        light.ambient = glm::vec3(1, 1, 1) * 0.2f;
        light.diffuse = glm::vec3(1, 1, 1) * 0.8f;
        light.specular = glm::vec3(1, 1, 1) * 1.0f;

        FragmentUniformBufferObject fragUBO{};
        fragUBO.dirLight = light;
        fragUBO.material = material;
        fragUBO.viewPos = glm::vec4(-m_camera.get_transform()->position, 0);


        m_matrixUBO[currentImage]->write_to_buffer(&ubo);
        m_matrixUBO[currentImage]->flush();

        m_fragUBO[currentImage]->write_to_buffer(&fragUBO);
        m_fragUBO[currentImage]->flush();
    }

    void MoonshineApp::create_dock_space() {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
// because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
// all active windows docked into it will lose their parent and become undocked.
// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

// DockSpace
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            static auto first_time = true;
            if (first_time) {
                first_time = false;

                ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
                ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

                // split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
                //   window ID to split, direction, fraction (between 0 and 1), the final two setting let's choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
                //                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, nullptr,
                                                                &dockspace_id);
                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr,
                                                                &dockspace_id);
                auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr,
                                                                 &dockspace_id);

                // we now dock our windows into the docking node we made above
                ImGui::DockBuilderDockWindow("Scene Graph", dock_id_left);
                ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                ImGui::DockBuilderDockWindow("Workspace", dock_id_down);
                ImGui::DockBuilderDockWindow("Lobby manager", dock_id_down);
                ImGui::DockBuilderDockWindow("Logs", dock_id_down);
                ImGui::DockBuilderDockWindow("Stats", dock_id_down);

                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        ImGui::End();

    }

} // moonshined