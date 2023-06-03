//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_MOONSHINEAPP_H
#define MOONSHINE_MOONSHINEAPP_H
#define GLFW_INCLUDE_VULKAN

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
#include "utils/Constants.h"
#include "utils/VkValidationLayerUtils.h"
#include "utils/VkUtils.h"
#include "utils/FileUtils.h"
#include "graphics/Window.h"
#include "graphics/Device.h"
#include "graphics/Pipeline.h"

namespace moonshine {

    const std::vector<Vertex> vertices = {
            {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    class MoonshineApp {
    private:
        // Members
        Window m_window = Window(APP_NAME, WIDTH, HEIGHT);
        Device m_device = Device(m_window);
        Pipeline m_pipeline = Pipeline(m_window, m_device);;

    public:

        void run();

        bool m_framebufferResized = false;
    private:

        VkCommandPool m_vkCommandPool;
        VkBuffer m_vertexBuffer;
        VkDeviceMemory m_vertexBufferMemory;
        std::vector<VkCommandBuffer> m_vkCommandBuffers;
        std::vector<VkSemaphore> m_vkImageAvailableSemaphores;
        std::vector<VkSemaphore> m_vkRenderFinishedSemaphores;
        std::vector<VkFence> m_vkInFlightFences;
        uint32_t m_currentFrame = 0;

    private:

        void initVulkan() {
            createCommandPool();
            createVertexBuffer();
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

            if (vkCreateCommandPool(m_device.getVkDevice(), &poolInfo, nullptr, &m_vkCommandPool) != VK_SUCCESS) {
                throw std::runtime_error("failed to create command pool!");
            }
        }
        
        void createVertexBuffer() {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = sizeof(vertices[0]) * vertices.size();
            bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(m_device.getVkDevice(), &bufferInfo, nullptr, &m_vertexBuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create vertex buffer!");
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(m_device.getVkDevice(), m_vertexBuffer, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            if (vkAllocateMemory(m_device.getVkDevice(), &allocInfo, nullptr, &m_vertexBufferMemory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate vertex buffer memory!");
            }

            vkBindBufferMemory(m_device.getVkDevice(), m_vertexBuffer, m_vertexBufferMemory, 0);

            void* data;
            vkMapMemory(m_device.getVkDevice(), m_vertexBufferMemory, 0, bufferInfo.size, 0, &data);
            memcpy(data, vertices.data(), (size_t) bufferInfo.size);
            vkUnmapMemory(m_device.getVkDevice(), m_vertexBufferMemory);
        }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(m_device.getVkPhysicalDevice(), &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) &&
                    (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                    return i;
                }
            }

            throw std::runtime_error("failed to find suitable memory type!");
        }

        void createCommandBuffer() {
            m_vkCommandBuffers.resize(moonshine::MAX_FRAMES_IN_FLIGHT);

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = m_vkCommandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = (uint32_t) m_vkCommandBuffers.size();

            if (vkAllocateCommandBuffers(m_device.getVkDevice(), &allocInfo, m_vkCommandBuffers.data()) != VK_SUCCESS) {
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
                    vkCreateFence(m_device.getVkDevice(), &fenceInfo, nullptr, &m_vkInFlightFences[i]) != VK_SUCCESS) {

                    throw std::runtime_error("failed to create synchronization objects for a frame!");
                }
            }
        }

        void mainLoop() {
            while (!m_window.shouldClose()) {
                glfwPollEvents();
                drawFrame();
            }

            vkDeviceWaitIdle(m_device.getVkDevice());
        }

        void drawFrame() {
            vkWaitForFences(m_device.getVkDevice(), 1, &m_vkInFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

            uint32_t imageIndex;
            VkResult result = vkAcquireNextImageKHR(m_device.getVkDevice(), m_pipeline.getSwapChain(), UINT64_MAX,
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

            if (vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submitInfo, m_vkInFlightFences[m_currentFrame]) !=
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

            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
                m_pipeline.recreateSwapChain();
            } else if (result != VK_SUCCESS) {
                throw std::runtime_error("failed to present swap chain image");
            }

            m_currentFrame = (m_currentFrame + 1) % moonshine::MAX_FRAMES_IN_FLIGHT;
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

            VkBuffer vertexBuffers[] = {m_vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
            
            vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
            vkCmdEndRenderPass(commandBuffer);

            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }

        void cleanup() {

            vkDestroyBuffer(m_device.getVkDevice(), m_vertexBuffer, nullptr);
            vkFreeMemory(m_device.getVkDevice(), m_vertexBufferMemory, nullptr);

            for (size_t i = 0; i < moonshine::MAX_FRAMES_IN_FLIGHT; i++) {
                vkDestroySemaphore(m_device.getVkDevice(), m_vkRenderFinishedSemaphores[i], nullptr);
                vkDestroySemaphore(m_device.getVkDevice(), m_vkImageAvailableSemaphores[i], nullptr);
                vkDestroyFence(m_device.getVkDevice(), m_vkInFlightFences[i], nullptr);
            }

            vkDestroyCommandPool(m_device.getVkDevice(), m_vkCommandPool, nullptr);
        }
    };

} // moonshine

#endif //MOONSHINE_MOONSHINEAPP_H
