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
#include "editor/EngineSystems.h"
#include "graphics/Gui.h"


namespace moonshine {

    class MoonshineApp {
    public:
        static Settings APP_SETTINGS;
    private:

        
        // Members
        //TODO: Check if it makes sense to only ever expose the renderer and have a getter for device and window
        Window m_window = Window(APP_NAME, WIDTH, HEIGHT);
        Device m_device = Device(m_window);
        Renderer m_renderer = Renderer(m_window, m_device);
        Gui m_gui = Gui(m_device, m_window);

        std::shared_ptr<DescriptorPool> m_global_pool{};
        std::shared_ptr<MaterialManager> m_materialManager;
        

        std::vector<std::unique_ptr<Buffer>> m_matrixUBO;
        std::vector<std::unique_ptr<Buffer>> m_fragUBO;

        Scene m_scene;
        Camera m_camera;

        std::unique_ptr<moonshine::SceneGraph> m_sceneGraph;

    public:

        MoonshineApp();

        void run();

        static void load_settings();

        static void save_settings();

    private:

        void init_vulkan();

        void main_loop();
        
        void update_uniform_buffer(uint32_t currentImage);

        void cleanup() {
            m_materialManager->clean_up();
            EngineSystems::get_instance().clean_up();

            save_settings();
        }

        void create_dock_space();

        void show_inspector();

    };

} // moonshine

#endif //MOONSHINE_MOONSHINEAPP_H
