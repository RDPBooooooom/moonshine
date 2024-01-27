//
// Created by marvi on 27.07.2023.
//

#include <vulkan/vulkan.h>
#include <stdexcept>
#include "Renderer.h"
#include "../utils/Constants.h"
#include "iostream"

namespace moonshine {

    Renderer::Renderer(Window &window, Device &device) : m_window{window}, m_device{device} {
        recreate_swap_chain();
        create_command_buffers();
    }

    Renderer::~Renderer() {
        free_command_buffers();
    }

    void Renderer::recreate_swap_chain() {
        auto extent = m_window.get_extent();
        m_window.m_framebuffer_resized = false;

        while (extent.width == 0 || extent.height == 0) {
            extent = m_window.get_extent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_device.get_vk_device());

        if (m_swap_chain == nullptr) {
            m_swap_chain = std::make_unique<SwapChain>(m_device, extent);
        } else {
            std::shared_ptr<SwapChain> old_swap_chain = std::move(m_swap_chain);
            m_swap_chain = std::make_unique<SwapChain>(m_device, extent, old_swap_chain);

            if (!old_swap_chain->compare_swap_formats(*m_swap_chain)) {
                throw std::runtime_error("Swap chain image or depth format has changed");
            }
        }
    }

    void Renderer::create_command_buffers() {
        m_vk_command_buffers.resize(m_swap_chain->image_count());

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = m_device.get_command_pool();
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = (uint32_t) m_vk_command_buffers.size();

        if (vkAllocateCommandBuffers(m_device.get_vk_device(), &alloc_info, m_vk_command_buffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    VkCommandBuffer Renderer::begin_frame() {
        assert(!m_is_frame_started && "Can't call begin_frame while already in progress!");

        VkResult result = m_swap_chain->acquire_next_image(&m_current_image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreate_swap_chain();
            return nullptr;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        m_is_frame_started = true;

        auto command_buffer = get_current_command_buffer();
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0; // Optional
        begin_info.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        return command_buffer;
    }

    void Renderer::end_frame() {
        assert(m_is_frame_started && "Can't call end_frame while frame is not in progress!");

        auto command_buffer = get_current_command_buffer();

        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        VkResult result = m_swap_chain->submit_command_buffers(&m_vk_command_buffers[m_current_image_index],
                                                               &m_current_image_index);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.m_framebuffer_resized) {
            m_window.m_framebuffer_resized = false;
            recreate_swap_chain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        m_is_frame_started = false;
        m_current_image_index = (m_current_image_index + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::begin_swap_chain_render_pass(VkCommandBuffer command_buffer) {
        assert(m_is_frame_started && "Can't call begin_swap_chain_render_pass while frame is not in progress!");
        assert(command_buffer == get_current_command_buffer() &&
               "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = m_swap_chain->get_render_pass();
        render_pass_info.framebuffer = m_swap_chain->get_framebuffers()[m_current_image_index];
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = m_swap_chain->get_swap_chain_extent();

        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clear_values[1].depthStencil = {1.0f, 0};
        render_pass_info.clearValueCount = clear_values.size();
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swap_chain->get_swap_chain_extent().width);
        viewport.height = static_cast<float>(m_swap_chain->get_swap_chain_extent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_swap_chain->get_swap_chain_extent();
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    }

    void Renderer::end_swap_chain_render_pass(VkCommandBuffer command_buffer) {
        assert(m_is_frame_started && "Can't call end_swap_chain_render_pass while frame is not in progress!");
        assert(command_buffer == get_current_command_buffer() &&
               "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(command_buffer);
    }

    void Renderer::free_command_buffers() {
        vkFreeCommandBuffers(
                m_device.get_vk_device(),
                m_device.get_command_pool(),
                static_cast<uint32_t>(m_vk_command_buffers.size()),
                m_vk_command_buffers.data());
        m_vk_command_buffers.clear();
    }

} // moonshine