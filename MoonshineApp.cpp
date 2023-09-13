//
// Created by marvi on 29.05.2023.
//

#include "MoonshineApp.h"
#include "external/imgui/imgui.h"
#include "external/imgui/backends/imgui_impl_glfw.h"
#include "external/imgui/backends/imgui_impl_vulkan.h"


namespace moonshine {

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
        initVulkan();
        initImGui();
        mainLoop();
        cleanup();
    }

    void MoonshineApp::initImGui() {
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
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
        init_info.DescriptorPool = globalPool->getVkDiscriptorPool();
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

} // moonshined