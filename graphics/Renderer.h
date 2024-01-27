//
// Created by marvi on 27.07.2023.
//

#ifndef MOONSHINE_RENDERER_H
#define MOONSHINE_RENDERER_H

#include "Window.h"
#include "Device.h"
#include "SwapChain.h"

namespace moonshine {

    class Renderer {

        Renderer &operator=(const Renderer &) = delete;

    private:
        Window &m_window;
        Device &m_device;
        std::unique_ptr<SwapChain> m_swap_chain;
        uint32_t m_current_image_index{0};
        bool m_is_frame_started = false;

        std::vector<VkCommandBuffer> m_vk_command_buffers;

    private:

        void create_command_buffers();

    public:

        Renderer(Window &window, Device &device);

        ~Renderer();

        VkCommandBuffer begin_frame();

        void end_frame();

        void begin_swap_chain_render_pass(VkCommandBuffer command_buffer);

        void end_swap_chain_render_pass(VkCommandBuffer command_buffer);

        bool is_frame_in_progress() const { return m_is_frame_started; }

        VkCommandBuffer get_current_command_buffer() const {
            assert(m_is_frame_started && "Cannot get command buffer when frame is not in progress");
            return m_vk_command_buffers[m_current_image_index];
        }

        VkRenderPass get_swap_chain_render_pass() const { return m_swap_chain->get_render_pass(); }

        VkExtent2D get_swap_chain_extent() { return m_swap_chain->get_swap_chain_extent(); }

        void recreate_swap_chain();

        void free_command_buffers();

        uint32_t get_frame_index() { return m_current_image_index; }
    };

} // moonshine

#endif //MOONSHINE_RENDERER_H
