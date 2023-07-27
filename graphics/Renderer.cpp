//
// Created by marvi on 27.07.2023.
//

#include <vulkan/vulkan.h>
#include <stdexcept>
#include "Renderer.h"
#include "../utils/Constants.h"

namespace moonshine {

    void Renderer::drawFrame() {
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

    Renderer::Renderer(Window &window, Device &device) : m_window{window}, m_device{device} {

    }

    void Renderer::createCommandBuffer() {
        m_vkCommandBuffers.resize(moonshine::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_device.getCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) m_vkCommandBuffers.size();

        if (vkAllocateCommandBuffers(m_device.getVkDevice(), &allocInfo, m_vkCommandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
} // moonshine