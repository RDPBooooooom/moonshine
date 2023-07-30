﻿//
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
#include "editor/Time.h"
#include "graphics/TextureImage.h"
#include "graphics/TextureSampler.h"
#include "graphics/Buffer.h"
#include "editor/SceneObject.h"
#include "graphics/Renderer.h"
#include "graphics/SimpleRenderSystem.h"
#include "graphics/Descriptors.h"
#include "graphics/UniformBuffer.h"

namespace moonshine {

    inline std::vector<Vertex> vertices = {
            {{-0.5f, 0.5f,  0.0f}, {1, 1, 1}, {1.0f, 0.0f}, {0,  0,  -1}}, // 0
            {{-0.5f, 0.5f,  0.0f}, {1, 1, 1}, {1.0f, 1.0f}, {0,  1,  0}}, // 1
            {{-0.5f, 0.5f,  0.0f}, {1, 1, 1}, {1.0f, 1.0f}, {-1, 0,  0}}, // 2
//front_top_right 1
            {{0.5f,  0.5f,  0.0f}, {1, 1, 1}, {1.0f, 1.0f}, {0,  0,  -1}}, // 3
            {{0.5f,  0.5f,  0.0f}, {1, 1, 1}, {1.0f, 0.0f}, {0,  1,  0}}, // 4
            {{0.5f,  0.5f,  0.0f}, {1, 1, 1}, {1.0f, 0.0f}, {1,  0,  0}}, // 5
// front_bot_left 2
            {{-0.5f, -0.5f, 0.0f}, {0, 0, 0}, {0.0f, 0.0f}, {0,  0,  -1}},  // 6
            {{-0.5f, -0.5f, 0.0f}, {0, 0, 0}, {1.0f, 0.0f}, {0,  -1, 0}},  // 7
            {{-0.5f, -0.5f, 0.0f}, {0, 0, 0}, {0.0f, 1.0f}, {-1, 0,  0}},  // 8
// front_bot_right 3
            {{0.5f,  -0.5f, 0.0f}, {0, 0, 0}, {0.0f, 1.0f}, {0,  0,  -1}},  // 9
            {{0.5f,  -0.5f, 0.0f}, {0, 0, 0}, {1.0f, 1.0f}, {0,  -1, 0}},  // 10
            {{0.5f,  -0.5f, 0.0f}, {0, 0, 0}, {0.0f, 0.0f}, {1,  0,  0}},  // 11
// back_top_left 4
            {{-0.5f, 0.5f,  1.0f}, {1, 1, 1}, {0.0f, 1.0f}, {0,  1,  0}}, // 12
            {{-0.5f, 0.5f,  1.0f}, {1, 1, 1}, {1.0f, 0.0f}, {-1, 0,  0}}, // 13
            {{-0.5f, 0.5f,  1.0f}, {1, 1, 1}, {1.0f, 1.0f}, {0,  0,  1}}, // 14
// back_top_right 5
            {{0.5f,  0.5f,  1.0f}, {1, 1, 1}, {0.0f, 0.0f}, {0,  1,  0}}, // 15
            {{0.5f,  0.5f,  1.0f}, {1, 1, 1}, {1.0f, 1.0f}, {1,  0,  0}}, // 16
            {{0.5f,  0.5f,  1.0f}, {1, 1, 1}, {1.0f, 0.0f}, {0,  0,  1}}, // 17
// back_bot_left 6
            {{-0.5f, -0.5f, 1.0f}, {0, 0, 0}, {0.0f, 0.0f}, {0,  -1, 0}}, // 18 
            {{-0.5f, -0.5f, 1.0f}, {0, 0, 0}, {0.0f, 0.0f}, {-1, 0,  0}}, // 19
            {{-0.5f, -0.5f, 1.0f}, {0, 0, 0}, {0.0f, 1.0f}, {0,  0,  1}}, // 20
// back_bot_right 7
            {{0.5f,  -0.5f, 1.0f}, {0, 0, 0}, {0.0f, 1.0f}, {0,  -1, 0}}, // 21 
            {{0.5f,  -0.5f, 1.0f}, {0, 0, 0}, {0.0f, 1.0f}, {1,  -0, 0}}, // 22
            {{0.5f,  -0.5f, 1.0f}, {0, 0, 0}, {0.0f, 0.0f}, {0,  0,  1}} // 23
    };

    inline std::vector<uint16_t> indices = {
            // Front
            6, 0, 9,
            9, 0, 3,
            // TOP
            1, 12, 4,
            4, 12, 15,
            // BOT
            18, 7, 21,
            21, 7, 10,
            // Right
            22, 11, 16,
            16, 11, 5,
            // LEFT
            13, 2, 19,
            19, 2, 8,
            // BACK
            14, 20, 17,
            17, 20, 23
    };


    class MoonshineApp {
    private:

        // Members
        Window m_window = Window(APP_NAME, WIDTH, HEIGHT);
        Device m_device = Device(m_window);
        Renderer m_renderer = Renderer(m_window, m_device);

        std::unique_ptr<DescriptorPool> globalPool{};

        std::unique_ptr<GpuBuffer<Vertex>> m_vertexBuffer;
        std::unique_ptr<GpuBuffer<uint16_t>> m_indexBuffer;

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

        void initVulkan() {
            globalPool = DescriptorPool::Builder(m_device).setMaxSets(MAX_FRAMES_IN_FLIGHT)
                    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT * 2)
                    .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT)
                    .build();
/*
            m_matrixUBONew = std::make_unique<UniformBuffer<UniformBufferObject>>(m_device);
            m_fragUBONew = std::make_unique<UniformBuffer<FragmentUniformBufferObject>>(m_device);
*/

            std::function<void()> mvObject = std::bind(&MoonshineApp::moveObject, this);
            m_window.getInputHandler()->registerKeyEvent(GLFW_KEY_Q, mvObject);
            std::function<void()> mvObjectTwo = std::bind(&MoonshineApp::moveObjectTwo, this);
            m_window.getInputHandler()->registerKeyEvent(GLFW_KEY_E, mvObjectTwo);
            std::function<void()> mvObjectThree = std::bind(&MoonshineApp::moveObjectThree, this);
            m_window.getInputHandler()->registerKeyEvent(GLFW_KEY_R, mvObjectThree);
            std::function<void()> newGameObj = std::bind(&MoonshineApp::addGameObject, this);
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
            m_vertexBuffer = std::make_unique<GpuBuffer<Vertex>>
                    (vertices, m_device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            m_indexBuffer = std::make_unique<GpuBuffer<uint16_t>>
                    (indices, m_device,
                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
            //m_image = std::make_unique<TextureImage>("../resources/textures/texture.jpg", &m_device,
            //                                         m_vkCommandPool);
            m_image = std::make_unique<TextureImage>("../resources/Models/Avocado/Avocado_baseColor.png", &m_device,
                                                     m_device.getCommandPool());
            m_sampler = std::make_unique<TextureSampler>(&m_device);

            std::cout << "opened Image and created Sampler \n";

            for (int i = 0; i < 100; ++i) {
                gameObjects.push_back(std::make_shared<SceneObject>("resources/Models/Avocado/Avocado.gltf", m_device));
                gameObjects[i]->getTransform()->position = glm::vec3(0 + i, 0, 0);
                gameObjects[i]->getTransform()->scale *= 20;
            }
            
        }

        void moveObject() {
            for (auto & i : gameObjects) {
                i->getTransform()->position += glm::vec3(0, 0, 1) * Time::deltaTime;
            }
        }

        void moveObjectTwo() {
            for (auto & i : gameObjects) {
                i->getTransform()->position += glm::vec3(0, 1, 0) * Time::deltaTime;
            }
        }

        void moveObjectThree() {
            for (auto & i : gameObjects) {
                i->getTransform()->position += glm::vec3(1, 0, 0) * Time::deltaTime;
            }
        }
        
        void addGameObject(){
            size_t i = gameObjects.size();
            gameObjects.push_back(std::make_shared<SceneObject>("resources/Models/Avocado/Avocado.gltf", m_device));
            gameObjects[i]->getTransform()->position = glm::vec3(0 + i, 0, 0);
            gameObjects[i]->getTransform()->scale *= 20;
        }

        void mainLoop() {
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

                /*
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = m_matrixUBONew->getUniformBuffer(i);
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = m_image->getImageView();
                imageInfo.sampler = m_sampler->getVkSampler();

                VkDescriptorBufferInfo fragBufferInfo{};
                fragBufferInfo.buffer = m_fragUBONew->getUniformBuffer(i);
                fragBufferInfo.offset = 0;
                fragBufferInfo.range = sizeof(FragmentUniformBufferObject);
*/
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

                    m_renderer.endSwapChainRenderPass(commandBuffer);
                    m_renderer.endFrame();
                }
            }

            vkDeviceWaitIdle(m_device.getVkDevice());
        }


        void updateUniformBuffer(uint32_t currentImage) {
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

        void cleanup() {
        }
    };

} // moonshine

#endif //MOONSHINE_MOONSHINEAPP_H
