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
#include "editor/Time.h"
#include "graphics/TextureImage.h"
#include "graphics/TextureSampler.h"
#include "graphics/UniformBuffer.h"
#include "editor/SceneObject.h"

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
        Pipeline m_pipeline = Pipeline(m_window, m_device);

        std::unique_ptr<GpuBuffer<Vertex>> m_vertexBuffer;
        std::unique_ptr<GpuBuffer<uint16_t>> m_indexBuffer;
        std::unique_ptr<TextureImage> m_image;
        std::unique_ptr<TextureSampler> m_sampler;

        SceneObject cube = SceneObject("resources/Models/Box.gltf");

        std::unique_ptr<UniformBuffer<UniformBufferObject>> m_matrixUBO;
        std::unique_ptr<UniformBuffer<FragmentUniformBufferObject>> m_fragUBO;

        VkCommandPool m_vkCommandPool;

        Camera m_camera;

    public:

        MoonshineApp();

        void run();

    private:

        VkDescriptorPool m_descriptorPool;
        std::vector<VkDescriptorSet> m_descriptorSets;

        std::vector<VkCommandBuffer> m_vkCommandBuffers;
        std::vector<VkSemaphore> m_vkImageAvailableSemaphores;
        std::vector<VkSemaphore> m_vkRenderFinishedSemaphores;
        std::vector<VkFence> m_vkInFlightFences;
        uint32_t m_currentFrame = 0;

    private:

        void initVulkan() {
            createCommandPool();

            m_vertexBuffer = std::make_unique<GpuBuffer<Vertex>>
                    (vertices, m_device, m_vkCommandPool,
                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            m_indexBuffer = std::make_unique<GpuBuffer<uint16_t>>
                    (indices, m_device, m_vkCommandPool,
                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
            //m_image = std::make_unique<TextureImage>("../resources/textures/texture.jpg", &m_device,
            //                                         m_vkCommandPool);
            m_image = std::make_unique<TextureImage>("../resources/textures/img.jpg", &m_device,
                                                     m_vkCommandPool);
            m_sampler = std::make_unique<TextureSampler>(&m_device);

            m_matrixUBO = std::make_unique<UniformBuffer<UniformBufferObject>>
                    (&m_device);
            m_fragUBO = std::make_unique<UniformBuffer<FragmentUniformBufferObject>>
                    (&m_device);

            createDescriptorPool();
            createDescriptorSets();
            createCommandBuffer();
            createSyncObjects();
        }

        void createCommandPool() {
            QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_device.getVkPhysicalDevice(),
                                                                      m_device.getVkSurface());

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

            if (vkCreateCommandPool(m_device.getVkDevice(), &poolInfo, nullptr, &m_vkCommandPool) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to create command pool!");
            }
        }

        void createDescriptorPool() {
            std::array<VkDescriptorPoolSize, 2> poolSizes{};
            poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();
            poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

            if (vkCreateDescriptorPool(m_device.getVkDevice(), &poolInfo, nullptr, &m_descriptorPool) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor pool!");
            }
        }

        void createDescriptorSets() {
            std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                                       m_pipeline.getDiscriptorSetLayout());
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = m_descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            allocInfo.pSetLayouts = layouts.data();

            m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
            if (vkAllocateDescriptorSets(m_device.getVkDevice(), &allocInfo, m_descriptorSets.data()) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = m_matrixUBO->getUniformBuffer(i);
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = m_image->getImageView();
                imageInfo.sampler = m_sampler->getVkSampler();

                VkDescriptorBufferInfo fragBufferInfo{};
                fragBufferInfo.buffer = m_fragUBO->getUniformBuffer(i);
                fragBufferInfo.offset = 0;
                fragBufferInfo.range = sizeof(FragmentUniformBufferObject);

                std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

                descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[0].dstSet = m_descriptorSets[i];
                descriptorWrites[0].dstBinding = 0;
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[0].descriptorCount = 1;
                descriptorWrites[0].pBufferInfo = &bufferInfo;

                descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[1].dstSet = m_descriptorSets[i];
                descriptorWrites[1].dstBinding = 1;
                descriptorWrites[1].dstArrayElement = 0;
                descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[1].descriptorCount = 1;
                descriptorWrites[1].pImageInfo = &imageInfo;

                descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[2].dstSet = m_descriptorSets[i];
                descriptorWrites[2].dstBinding = 2;
                descriptorWrites[2].dstArrayElement = 0;
                descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[2].descriptorCount = 1;
                descriptorWrites[2].pBufferInfo = &fragBufferInfo;

                vkUpdateDescriptorSets(m_device.getVkDevice(), static_cast<uint32_t>(descriptorWrites.size()),
                                       descriptorWrites.data(), 0, nullptr);
            }
        }

        void createCommandBuffer() {
            m_vkCommandBuffers.resize(moonshine::MAX_FRAMES_IN_FLIGHT);

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = m_vkCommandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = (uint32_t) m_vkCommandBuffers.size();

            if (vkAllocateCommandBuffers(m_device.getVkDevice(), &allocInfo, m_vkCommandBuffers.data()) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to allocate command buffers!");
            }
        }

        void createSyncObjects() {
            m_vkImageAvailableSemaphores.resize(moonshine::MAX_FRAMES_IN_FLIGHT);
            m_vkRenderFinishedSemaphores.resize(moonshine::MAX_FRAMES_IN_FLIGHT);
            m_vkInFlightFences.resize(moonshine::MAX_FRAMES_IN_FLIGHT);

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (size_t i = 0; i < moonshine::MAX_FRAMES_IN_FLIGHT; i++) {
                if (vkCreateSemaphore(m_device.getVkDevice(), &semaphoreInfo, nullptr,
                                      &m_vkImageAvailableSemaphores[i]) !=
                    VK_SUCCESS ||
                    vkCreateSemaphore(m_device.getVkDevice(), &semaphoreInfo, nullptr,
                                      &m_vkRenderFinishedSemaphores[i]) !=
                    VK_SUCCESS ||
                    vkCreateFence(m_device.getVkDevice(), &fenceInfo, nullptr, &m_vkInFlightFences[i]) !=
                    VK_SUCCESS) {

                    throw std::runtime_error("failed to create synchronization objects for a frame!");
                }
            }
        }

        void mainLoop() {
            Time::initTime();
            while (!m_window.shouldClose()) {
                Time::calcDeltaTime();
                glfwPollEvents();
                m_window.getInputHandler()->triggerEvents();
                drawFrame();
            }

            vkDeviceWaitIdle(m_device.getVkDevice());
        }

        void drawFrame() {
            vkWaitForFences(m_device.getVkDevice(), 1, &m_vkInFlightFences[m_currentFrame], VK_TRUE,
                            UINT64_MAX);

            updateUniformBuffer(m_currentFrame);

            uint32_t imageIndex;
            VkResult result = vkAcquireNextImageKHR(m_device.getVkDevice(), m_pipeline.getSwapChain(),
                                                    UINT64_MAX,
                                                    m_vkImageAvailableSemaphores[m_currentFrame],
                                                    VK_NULL_HANDLE,
                                                    &imageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                m_pipeline.recreateSwapChain();
                return;
            } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                throw std::runtime_error("failed to acquire swap chain image");
            }

            // Only reset the fence if we are submitting work
            vkResetFences(m_device.getVkDevice(), 1, &m_vkInFlightFences[m_currentFrame]);

            vkResetCommandBuffer(m_vkCommandBuffers[m_currentFrame], 0);
            recordCommandBuffer(m_vkCommandBuffers[m_currentFrame], imageIndex);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore waitSemaphores[] = {m_vkImageAvailableSemaphores[m_currentFrame]};
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &m_vkCommandBuffers[m_currentFrame];

            VkSemaphore signalSemaphores[] = {m_vkRenderFinishedSemaphores[m_currentFrame]};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            if (vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submitInfo,
                              m_vkInFlightFences[m_currentFrame]) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to submit draw command buffer!");
            }

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;

            VkSwapchainKHR swapChains[] = {m_pipeline.getSwapChain()};
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr; // Optional

            result = vkQueuePresentKHR(m_device.getPresentQueue(), &presentInfo);

            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
                m_window.m_framebufferResized) {
                m_pipeline.recreateSwapChain();
            } else if (result != VK_SUCCESS) {
                throw std::runtime_error("failed to present swap chain image");
            }

            m_currentFrame = (m_currentFrame + 1) % moonshine::MAX_FRAMES_IN_FLIGHT;
        }

        void updateUniformBuffer(uint32_t currentImage) {
            static auto startTime = std::chrono::high_resolution_clock::now();

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(
                    currentTime - startTime).count();

            UniformBufferObject ubo{};

            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            ubo.view = m_camera.getViewMat();
            //ubo.view = glm::lookAt(glm::vec3(0.0f, 2.5f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f),
            //                       glm::vec3(0.0f, 0.0f, 1.0f));
            ubo.proj = glm::perspective(glm::radians(45.0f), m_pipeline.getSwapChainExtent().width /
                                                             (float) m_pipeline.getSwapChainExtent().height,
                                        0.1f,
                                        10.0f);
            ubo.tangentToWorld = glm::transpose(glm::inverse(
                    glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f))));

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
            fragUBO.viewPos = m_camera.getTransform()->position;

            memcpy(m_matrixUBO->getMappedUniformBuffer(currentImage), &ubo, sizeof(ubo));
            memcpy(m_fragUBO->getMappedUniformBuffer(currentImage), &fragUBO, sizeof(fragUBO));
        }

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_pipeline.getRenderPass();
            renderPassInfo.framebuffer = m_pipeline.getFramebuffers()[imageIndex];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = m_pipeline.getSwapChainExtent();

            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.getGraphicsPipeline());

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(m_pipeline.getSwapChainExtent().width);
            viewport.height = static_cast<float>(m_pipeline.getSwapChainExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = m_pipeline.getSwapChainExtent();
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            VkBuffer vertexBuffers[] = {m_vertexBuffer->getBuffer()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_pipeline.getPipelineLayout(), 0,
                                    1, &m_descriptorSets[m_currentFrame], 0, nullptr);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
            vkCmdEndRenderPass(commandBuffer);

            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }

        void cleanup() {
            vkDestroyDescriptorPool(m_device.getVkDevice(), m_descriptorPool, nullptr);

            for (size_t i = 0; i < moonshine::MAX_FRAMES_IN_FLIGHT; i++) {
                vkDestroySemaphore(m_device.getVkDevice(), m_vkRenderFinishedSemaphores[i], nullptr);
                vkDestroySemaphore(m_device.getVkDevice(), m_vkImageAvailableSemaphores[i], nullptr);
                vkDestroyFence(m_device.getVkDevice(), m_vkInFlightFences[i], nullptr);
            }

            vkDestroyCommandPool(m_device.getVkDevice(), m_vkCommandPool, nullptr);

            m_vertexBuffer = nullptr;
            m_indexBuffer = nullptr;
        }
    };

} // moonshine

#endif //MOONSHINE_MOONSHINEAPP_H
