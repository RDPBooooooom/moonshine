//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_MOONSHINEAPP_H
#define MOONSHINE_MOONSHINEAPP_H
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS

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
#include "moonshine/editor/Settings.h"

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

        std::mutex editGameObjectsMutex;
        std::vector<std::shared_ptr<SceneObject>> gameObjects;

        std::vector<std::unique_ptr<Buffer>> m_matrixUBO;
        std::vector<std::unique_ptr<Buffer>> m_fragUBO;
/*
        std::unique_ptr<UniformBuffer<UniformBufferObject>> m_matrixUBONew;
        std::unique_ptr<UniformBuffer<FragmentUniformBufferObject>> m_fragUBONew;
*/
        std::unique_ptr<TextureImage> m_image;
        std::unique_ptr<TextureSampler> m_sampler;

        Camera m_camera;

    public:

        MoonshineApp();

        void run();


    private:

        void initVulkan();

        std::string getExecutablePath() {
            char buffer[MAX_PATH];
            GetModuleFileName(NULL, buffer, MAX_PATH);
            std::string::size_type pos = std::string(buffer).find_last_of("\\/");
            return std::string(buffer).substr(0, pos);
        }

        void moveObject(bool isReleased) {
            for (auto &i: gameObjects) {
                i->getTransform()->position += glm::vec3(0, 0, 1) * Time::deltaTime;
            }
        }

        void moveObjectTwo(bool isReleased) {
            for (auto &i: gameObjects) {
                i->getTransform()->position += glm::vec3(0, 1, 0) * Time::deltaTime;
            }
        }

        void moveObjectThree(bool isReleased) {
            for (auto &i: gameObjects) {
                i->getTransform()->position += glm::vec3(1, 0, 0) * Time::deltaTime;
            }
        }

        void loadAvocado() {
            size_t i = gameObjects.size();
            std::shared_ptr<SceneObject> obj = std::make_shared<SceneObject>("resources/Models/Avocado/Avocado.gltf");
            obj->getTransform()->position = glm::vec3(0 + i, 0, 0);
            obj->getTransform()->scale *= 20;
            std::lock_guard<std::mutex> lock(editGameObjectsMutex);
            obj->initBuffer(m_device);

            
            gameObjects.push_back(obj);
        }
        
        void addGameObject(bool isReleased) {
            std::function<void()> handleLoadAvocado = std::bind(&MoonshineApp::loadAvocado, this);
            std::thread t(handleLoadAvocado);
            t.detach();
        }

        void mainLoop();


        void updateUniformBuffer(uint32_t currentImage);

        void cleanup() {
            //TODO Destroy descriptor set for imgui
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        void initImGui();

        void loadSettings();
    };

} // moonshine

#endif //MOONSHINE_MOONSHINEAPP_H
