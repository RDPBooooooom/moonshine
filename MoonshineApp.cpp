﻿//
// Created by marvi on 29.05.2023.
//

#include "MoonshineApp.h"
#include "external/imgui/imgui.h"
#include "external/imgui/backends/imgui_impl_glfw.h"
#include "external/imgui/backends/imgui_impl_vulkan.h"
#include "editor/Settings.h"


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
                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT)
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
        m_window.getInputHandler()->registerKeyEvent(GLFW_KEY_T, newGameObj);

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
        };

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
        };

        std::cout << "FRAG UBO created \n";

        //m_image = std::make_unique<TextureImage>("../resources/textures/texture.jpg", &m_device,
        //                                         m_vkCommandPool);
        m_image = std::make_unique<TextureImage>(
                (getExecutablePath() + "/resources/Models/Avocado/Avocado_baseColor.png").c_str(), &m_device,
                m_device.getCommandPool());
        m_sampler = std::make_unique<TextureSampler>(&m_device);

        std::cout << "opened Image and created Sampler \n";

        for (int i = 0; i < 5; ++i) {
            gameObjects.push_back(std::make_shared<SceneObject>("resources/Models/Avocado/Avocado.gltf", m_device));
            gameObjects[i]->getTransform()->position = glm::vec3(0 + i, 0, 0);
            gameObjects[i]->getTransform()->scale *= 20;
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

        VkDescriptorPool imguiPool;
        check_vk_result(vkCreateDescriptorPool(m_device.getVkDevice(), &pool_info, nullptr, &imguiPool));

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
        init_info.DescriptorPool = imguiPool;
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
                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .addBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();
        std::vector<VkDescriptorSet> globalDescriptorSets(MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < globalDescriptorSets.size(); i++) {

            auto bufferInfo = m_matrixUBO[i]->descriptorInfo();

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = m_image->getImageView();
            imageInfo.sampler = m_sampler->getVkSampler();

            auto fragBufferInfo = m_fragUBO[i]->descriptorInfo();

            DescriptorWriter(*globalSetLayout, *globalPool)
                    .writeBuffer(0, &bufferInfo)
                    .writeImage(1, &imageInfo)
                    .writeBuffer(2, &fragBufferInfo)
                    .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{m_device, m_renderer.getSwapChainRenderPass(),
                                              globalSetLayout->getDescriptorSetLayout()};

        while (!m_window.shouldClose()) {
            Time::calcDeltaTime();
            glfwPollEvents();

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            m_window.getInputHandler()->triggerEvents();

            if (auto commandBuffer = m_renderer.beginFrame()) {

                int frameIndex = m_renderer.getFrameIndex();
                updateUniformBuffer(frameIndex);

                m_renderer.beginSwapChainRenderPass(commandBuffer);

                FrameInfo frameInfo{
                        frameIndex,
                        Time::deltaTime,
                        commandBuffer,
                        m_camera,
                        globalDescriptorSets[frameIndex]};

                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);

                ImGui::Render();
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

                m_renderer.endSwapChainRenderPass(commandBuffer);
                m_renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(m_device.getVkDevice());
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

} // moonshined