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
        loadSettings();
    }

    void MoonshineApp::run() {
        initVulkan();
        initImGui();
        mainLoop();
        cleanup();
    }


    void MoonshineApp::loadSettings() {
        APP_SETTINGS.ENABLE_MOUSE_DEBUG = true;
    }

    void MoonshineApp::initVulkan() {
        globalPool = DescriptorPool::Builder(m_device).setMaxSets(MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT * 2)
                .build();
        
        materialPool = DescriptorPool::Builder(m_device).setMaxSets(MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_MATERIALS / 2)
                .build();
/*
            m_matrixUBONew = std::make_unique<UniformBuffer<UniformBufferObject>>(m_device);
            m_fragUBONew = std::make_unique<UniformBuffer<FragmentUniformBufferObject>>(m_device);
*/
        using std::placeholders::_1;
        std::function<void(bool)> mvObject = std::bind(&MoonshineApp::moveObject, this, _1);
        m_window.getInputHandler()->registerKeyEvent(GLFW_KEY_Q, mvObject);
        std::function<void(bool)> mvObjectTwo = std::bind(&MoonshineApp::moveObjectTwo, this, _1);
        m_window.getInputHandler()->registerKeyEvent(GLFW_KEY_E, mvObjectTwo);
        std::function<void(bool)> mvObjectThree = std::bind(&MoonshineApp::moveObjectThree, this, _1);
        m_window.getInputHandler()->registerKeyEvent(GLFW_KEY_R, mvObjectThree);
        std::function<void(bool)> newGameObj = std::bind(&MoonshineApp::addGameObject, this, _1);
        m_window.getInputHandler()->registerKeyEvent(GLFW_KEY_T, newGameObj, false, false);

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

        std::cout << "UBO created \n";

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

        std::cout << "FRAG UBO created \n";

        //m_image = std::make_unique<TextureImage>("../resources/textures/texture.jpg", &m_device,
        //                                         m_vkCommandPool);
        m_image = std::make_unique<TextureImage>(
                (getExecutablePath() + "/resources/Models/Avocado/Avocado_baseColor.png").c_str(), &m_device,
                m_device.getCommandPool());
        m_sampler = std::make_unique<TextureSampler>(&m_device);

        std::cout << "opened Image and created Sampler \n";

        for (int i = 0; i < 5; ++i) {
            gameObjects.push_back(std::make_shared<SceneObject>("resources/Models/Avocado/Avocado.gltf"));
            gameObjects[i]->getTransform()->position = glm::vec3(0 + i, 0, 0);
            gameObjects[i]->getTransform()->scale *= 20;
            gameObjects[i]->initBuffer(m_device);
        }

    }

    void MoonshineApp::initImGui() {
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

        check_vk_result(vkCreateDescriptorPool(m_device.getVkDevice(), &pool_info, nullptr, &m_imGuiPool));

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.Fonts->AddFontDefault();

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(m_window.getGLFWWindow(), true);

        // Setup Platform/Renderer backends
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_device.getVkInstance();
        init_info.PhysicalDevice = m_device.getVkPhysicalDevice();
        init_info.Device = m_device.getVkDevice();
        init_info.QueueFamily = m_device.getGraphicsQueueFamily();
        init_info.Queue = m_device.getGraphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = m_imGuiPool;
        init_info.Subpass = 0;
        init_info.MinImageCount = 2;
        init_info.ImageCount = 2;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = VK_NULL_HANDLE;
        init_info.CheckVkResultFn = check_vk_result;

        ImGui_ImplVulkan_Init(&init_info, m_renderer.getSwapChainRenderPass());


        VkCommandBuffer commandBuffer = beginSingleTimeCommands(&m_device, m_device.getCommandPool());
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        endSingleTimeCommands(&m_device, commandBuffer, m_device.getCommandPool());

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    void MoonshineApp::mainLoop() {
        Time::initTime();

        auto globalSetLayout = DescriptorSetLayout::Builder(m_device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();
        std::vector<VkDescriptorSet> globalDescriptorSets(MAX_FRAMES_IN_FLIGHT);
        
        auto materialSetLayout = DescriptorSetLayout::Builder(m_device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();
        std::vector<VkDescriptorSet> materialDescriptorSets(MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < globalDescriptorSets.size(); i++) {

            auto bufferInfo = m_matrixUBO[i]->descriptorInfo();

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = m_image->getImageView();
            imageInfo.sampler = m_sampler->getVkSampler();

            auto fragBufferInfo = m_fragUBO[i]->descriptorInfo();

            DescriptorWriter(*globalSetLayout, *globalPool)
                    .writeBuffer(0, &bufferInfo)
                    .writeBuffer(1, &fragBufferInfo)
                    .build(globalDescriptorSets[i]);
        }

        for (int i = 0; i < materialDescriptorSets.size(); i++) {

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = m_image->getImageView();
            imageInfo.sampler = m_sampler->getVkSampler();

            DescriptorWriter(*materialSetLayout, *materialPool)
                    .writeImage(0, &imageInfo)
                    .build(materialDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{m_device, m_renderer.getSwapChainRenderPass(),
                                              globalSetLayout->getDescriptorSetLayout(), materialSetLayout->getDescriptorSetLayout()};

        while (!m_window.shouldClose()) {
            Time::calcDeltaTime();
            glfwPollEvents();

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            createDockSpace();

            m_window.getInputHandler()->triggerEvents();

            editGameObjectsMutex.lock();
            showSceneGraph();
            showInspector();
            if (auto commandBuffer = m_renderer.beginFrame()) {

                int frameIndex = m_renderer.getFrameIndex();
                updateUniformBuffer(frameIndex);

                m_renderer.beginSwapChainRenderPass(commandBuffer);

                FrameInfo frameInfo{
                        frameIndex,
                        Time::deltaTime,
                        commandBuffer,
                        m_camera,
                        globalDescriptorSets[frameIndex],
                        materialDescriptorSets[frameIndex]};

                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects, &editGameObjectsMutex);

                ImGui::Render();
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

                m_renderer.endSwapChainRenderPass(commandBuffer);
                m_renderer.endFrame();
            }
            editGameObjectsMutex.unlock();
        }

        vkDeviceWaitIdle(m_device.getVkDevice());
    }

    void MoonshineApp::showSceneGraph() {
        ImGui::Begin("Scene Graph");

        int index = 0;
        for (auto &item: gameObjects) {
            std::string uniqueName = item->getName() + "##" + std::to_string(index++);

            bool isOpen = ImGui::TreeNode(uniqueName.c_str());
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Select")) {
                    selectedGameObject = item;
                }
                if (ImGui::MenuItem("Rename")) {
                    openPopup = true;
                    popupItem = item;
                }

                if (ImGui::MenuItem("Delete")) { /* Handle delete... */ }
                ImGui::EndPopup();
            }

            if (isOpen) {
                ImGui::TreePop();
            }

        }

        showPopup(popupItem);
        ImGui::End();

    }

    void MoonshineApp::showInspector() {
        ImGui::Begin("Inspector");

        if (selectedGameObject != nullptr) {
            ImGui::Text(selectedGameObject->getName().c_str());

            ImGui::Text("Transform");
            ImGui::InputFloat3("Position", glm::value_ptr(selectedGameObject->getTransform()->position));
            glm::vec3 rotEulerAngles = eulerAngles(selectedGameObject->getTransform()->rotation);
            if (ImGui::InputFloat3("Rotation", glm::value_ptr(rotEulerAngles))) {
                selectedGameObject->getTransform()->rotation = glm::quat(rotEulerAngles);
            }
            ImGui::InputFloat3("Scale", glm::value_ptr(selectedGameObject->getTransform()->scale));
        }
        ImGui::End();
    }

    void MoonshineApp::showPopup(std::shared_ptr<SceneObject> &item) {
        if (!item) return;

        if (openPopup) {
            ImGui::OpenPopup(("Rename " + item->getName()).c_str());
            m_window.getInputHandler()->disable();
            openPopup = false;
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

                popupItem = nullptr;
                m_window.getInputHandler()->enable();
            }
            ImGui::EndPopup();
        }
    }

    void MoonshineApp::updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                currentTime - startTime).count();

        UniformBufferObject ubo{};

        ubo.view = m_camera.getViewMat();
        ubo.proj = glm::perspective(glm::radians(45.0f), m_renderer.getSwapChainExtent().width /
                                                         (float) m_renderer.getSwapChainExtent().height,
                                    0.1f,
                                    100.0f);

        /*
         * GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.
         * The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis 
         * in the projection matrix. If you don't do this, then the image will be rendered upside down.
         */
        ubo.proj[1][1] *= -1;

        Material material{};
        DirLight light{};
        light.direction = glm::normalize(glm::vec3(0, 1, -1));
        light.ambient = glm::vec3(1, 1, 1) * 0.2f;
        light.diffuse = glm::vec3(1, 1, 1) * 0.8f;
        light.specular = glm::vec3(1, 1, 1) * 1.0f;

        FragmentUniformBufferObject fragUBO{};
        fragUBO.dirLight = light;
        fragUBO.material = material;
        fragUBO.viewPos = glm::vec4(-m_camera.getTransform()->position, 0);


        m_matrixUBO[currentImage]->writeToBuffer(&ubo);
        m_matrixUBO[currentImage]->flush();

        m_fragUBO[currentImage]->writeToBuffer(&fragUBO);
        m_fragUBO[currentImage]->flush();

        /*memcpy(m_matrixUBONew->getMappedUniformBuffer(currentImage), &ubo, sizeof(ubo));
        m_matrixUBONew->flush(currentImage, VK_WHOLE_SIZE, 0);
        memcpy(m_fragUBONew->getMappedUniformBuffer(currentImage), &fragUBO, sizeof(fragUBO));
        m_fragUBONew->flush(currentImage, VK_WHOLE_SIZE, 0);*/
    }

    void MoonshineApp::createDockSpace() {
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
                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr,
                                                                &dockspace_id);
                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr,
                                                                &dockspace_id);
                auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.15f, nullptr,
                                                                 &dockspace_id);

                // we now dock our windows into the docking node we made above
                ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                ImGui::DockBuilderDockWindow("Scene Graph", dock_id_left);
                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        ImGui::End();

    }

} // moonshined