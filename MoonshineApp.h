//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_MOONSHINEAPP_H
#define MOONSHINE_MOONSHINEAPP_H
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS

#include "net/LobbyConnector.h"
#include <vulkan/vulkan_core.h>
#include <vector>
#include <stdexcept>
#include <map>
#include <optional>
#include <set>
#include <functional>
#include <limits>
#include <iostream>
#include <GLFW/glfw3.h>
#include <memory>
#include <chrono>
#include "utils/Constants.h"
#include "utils/VkValidationLayerUtils.h"
#include "utils/VkUtils.h"
#include "utils/FileUtils.h"
#include "graphics/Window.h"
#include "graphics/Device.h"
#include "graphics/Pipeline.h"
#include "graphics/GpuBuffer.h"
#include "utils/BufferUtils.h"
#include "editor/Camera.h"
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/common.hpp>
#include <thread>
#include <mutex>
#include "editor/Time.h"
#include "graphics/TextureImage.h"
#include "graphics/TextureSampler.h"
#include "graphics/Buffer.h"
#include "editor/SceneObject.h"
#include "graphics/Renderer.h"
#include "graphics/SimpleRenderSystem.h"
#include "graphics/Descriptors.h"
#include "graphics/UniformBuffer.h"
#include "external/imgui/imgui.h"
#include "external/imgui/backends/imgui_impl_glfw.h"
#include "external/imgui/backends/imgui_impl_vulkan.h"
#include "editor/Settings.h"
#include "graphics/MaterialManager.h"
#include "editor/ui/SceneGraph.h"
#include "editor/ui/LobbyManager.h"
#include "editor/Scene.h"
#include "editor/ui/WorkspaceManager.h"


namespace moonshine {

    class MoonshineApp {
    public:
        static Settings APP_SETTINGS;
    private:

        // Members
        Window m_window = Window(APP_NAME, WIDTH, HEIGHT);
        Device m_device = Device(m_window);
        Renderer m_renderer = Renderer(m_window, m_device);

        std::shared_ptr<DescriptorPool> globalPool{};
        std::shared_ptr<MaterialManager> m_materialManager;
        VkDescriptorPool m_imGuiPool;

        std::vector<std::unique_ptr<Buffer>> m_matrixUBO;
        std::vector<std::unique_ptr<Buffer>> m_fragUBO;

        Scene scene;
        Camera m_camera;

        std::unique_ptr<moonshine::SceneGraph> m_sceneGraph;
        
    public:

        MoonshineApp();

        void run();

        static void loadSettings();

        static void saveSettings();

    private:

        void initVulkan();

        void mainLoop();


        void updateUniformBuffer(uint32_t currentImage);

        void cleanup() {
            m_materialManager->clean_up();

            vkDestroyDescriptorPool(m_device.getVkDevice(), m_imGuiPool, nullptr);
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            saveSettings();
        }

        void initImGui();

        void createDockSpace();

        void showInspector();

    };

} // moonshine

#endif //MOONSHINE_MOONSHINEAPP_H
